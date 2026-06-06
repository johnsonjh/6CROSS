#!/usr/bin/env python3
"""Convert CP-6 FORTRAN single-character Hollerith constants (1Hx) into
the assembler's canonical packed-word integer representation: a single
character lives left-justified in the top 9 bits of a 36-bit word, i.e.
value = ICHAR(x) << 27.

Operates on fixed-form FORTRAN: skips comment lines, respects quoted
strings, and only treats `1H<c>` as a Hollerith when the `1` is not
preceded by an alphanumeric (so multi-digit Hollerith counts are left
alone -- though this toolchain only uses 1H).
"""
import sys


def is_comment(line):
    return bool(line) and line[0] in "Cc*!dD"


def process_line(line):
    if is_comment(line):
        return line, 0
    out = []
    i, n, count = 0, len(line), 0
    quote = None
    while i < n:
        ch = line[i]
        if quote:
            out.append(ch)
            if ch == quote:
                if i + 1 < n and line[i + 1] == quote:  # doubled-quote escape
                    out.append(line[i + 1])
                    i += 2
                    continue
                quote = None
            i += 1
            continue
        if ch in "'\"":
            quote = ch
            out.append(ch)
            i += 1
            continue
        # Hollerith must start at col 7+ (i>=6). At col 7 (i==6) the
        # preceding char is the column-6 continuation marker, not a token,
        # so skip the "preceded by alnum" guard there.
        if (
            ch == "1"
            and i + 2 < n
            and line[i + 1] in "Hh"
            and i >= 6
            and (i == 6 or not line[i - 1].isalnum())
        ):
            out.append(str(ord(line[i + 2]) << 27))
            i += 3
            count += 1
            continue
        out.append(ch)
        i += 1
    return "".join(out), count


def main():
    total = 0
    for path in sys.argv[1:]:
        with open(path) as f:
            lines = f.read().split("\n")
        res = []
        for ln in lines:
            new, c = process_line(ln)
            res.append(new)
            total += c
        with open(path, "w") as f:
            f.write("\n".join(res))
        print(
            f"{path}: converted {sum(process_line(l)[1] for l in lines)} Hollerith constants"
        )
    print(f"total converted: {total}")


if __name__ == "__main__":
    main()
