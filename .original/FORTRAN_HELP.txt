The FORTRAN compiler is invoked by the FORTRAN command taken at the IBEX 
level: 
    FORTRAN [source][,update] [{ON|OVER|INTO} [object] 
    [,listout]][(optionlist)] 
Parameters: 
FORTRAN    invokes the FORTRAN compiler. 
source    specifies the fid that contains the FORTRAN source code.  (In CP-6, 
"fids" are used to refer both to disk and tape files, as well as devices such 
as terminals, printers, etc.  See the Programmer Reference Manual (CE40), for 
information concerning fid format and use.) 
update    specifies the fid that contains updates to the source code. 
ON    specifies that if an object or listout file already exists, the existing 
file is not to be overwritten and the command is to be aborted. 
OVER    specifies that if an object or listout file already exists, the 
existing file is to be overwritten and the command is to be processed. 
INTO    specifies that if an object or listout file already exists, new output 
is to be added to the existing file.  If either of these files does not exist, 
it is created. 
object    specifies the disk fid to contain the generated object code. 
listout    specifies the fid to which the listing output should be routed. 
optionlist    specifies one or more of the compiler options in the format 
(option[,option...]). 
  ************************************************************** 
  *   Copyright, (c) Bull HN Information Systems Inc., 1992 
  ************************************************************** 
Syntax: 
ABNORMAL I1,I2,I3,...In 
Parameters: 
Ij    is an external function name. 
n > = 0. 
Description: 
The ABNORMAL statement is provided only for compatibility with other (and 
possibly future) FORTRAN systems.  Its purpose is to declare external 
functions that must be referenced exactly where (and as many times as) they 
are written in the program; i.e., the compiler must not optimize the object 
code associated with them.  However, the compiler currently treats all 
functions as abnormal, so the statement has no effect.  If there were such a 
thing as a "normal" function, it would then be necessary to declare as 
ABNORMAL any functions that performed any of the following: 
1.  Modified any of their calling arguments. 
2.  Modified a COMMON or GLOBAL variable. 
3.  Performed any sort of input/output. 
4.  Returned different values when called with the same arguments. 
5.  Kept a chronological count or history. 
00002
00001
Syntax: 
CALL ABORTSET [(loc[,level])] 
Parameters: 
loc    is a statement number followed by the letter S or preceded by an 
ampersand (&) or an asterisk (*) (or an ASSIGNed variable) to which control 
will be transferred (instead of aborting) when an abort level error occurs.  A 
value of zero (instead of a statement number) resets the system to abort as 
usual. 
level    is an integer value from 1 to 15.  The library aborts on any run-time 
error whose severity is greater than or equal to this value, or transfers to 
loc on any run-time error where severity is less than this value. 
Description: 
When called with only one argument, the abort level is unchanged.  This 
routine enablies the user to abort on any error, for example, or to take 
control himself and try to recover when a drastic errors occurs.  This does 
not apply to monitor-detected errors when control is not returned to the 
run-time routines.  When called with no arguments, this resets the system to 
abort as usual. 
00002
00001
In addition to the intrinsic and generic functions the Bull FORTRAN-77 library 
provides the following library subprograms: 
    BUFFIN 
    BUFFERIN 
    BUFFOUT 
    BUFFEROUT 
    ICHECK 
    EXIT 
    SSWTCH or SSWITCH 
    SLITET 
    SLITE 
    OVERFL 
    DVCHK 
    IF 
    DUMP 
    PDUMP 
    ABORTSET 
    EOFSET 
    ERROROUT 
    PROMPT 
    VFCP6 
    TRUNCATE 
To obtain additional information about any of above (such as ABORTSET), enter: 
HELP (FORTRAN) ABORTSET 
The adjustable format (N) specification is one of the most powerful features 
of FORTRAN-77.  It often eliminates the need to write a great number of FORMAT 
statements in order to handle slightly different situations.  Furthermore, it 
facilitates the input of records whose form is highly variable, and which 
could not be processed without this feature. 
Any of the quantities r, w, d, e, or i (see above) may be replaced by the 
letter N in a format specification.  When an N is encountered, its value is 
obtained from the next input or output list item.  The letter N is merely a 
form of specification and does not conflict with any variable, subprogram, 
etc., whose identifier may be N.  Also there is no limit to the number of N 
characters that may be used in a FORMAT statement or to the number of 
quantities replaced by N in a format specification. 
Example: 
    32 FORMAT (NX,FN.4,N(3X,E.5),NP,NGN.N) 
is a valid statement, and seven values will be taken from the list to satisfy 
the adjustable format specifications (N). 
The following set of rules defines the manner in which the value of N must be 
specified in a list and the way in which the values are utilized: 
1.  Integer, real, double precision, or either part of complex data may be 
    supplied as values for N.  Non-integer data are truncated to integer 
    value. 
2.  w and d (width and decimal point) specifications may be replaced only by 
    N, whereas r (repeat count) and i (skip or scale factor count) 
    specifications may be replaced by N or -N. 
3.  The resultant value (negated if preceded by a minus sign) may be negative 
    only when N is used to replace i. 
4.  When N appears one or more times in a single specification, its values 
    must appear sequentially in the list and prior to the items (if any) that 
    are to be processed by the specification.  An example is the list 
        3,4,1,A,B,C,12,-2,D   Where A,B,C, and D are variables 
    and the statement 
        3 FORMAT(NEN.N,NX,NP,G14.8) 
    which are equivalent to the list 
        A,B,C,D 
    and the statement 
        3 FORMAT(3E4.1,12X,-2P,G14.8) 
5.  Whenever N is used with a specification that is enclosed in 
    repetition-type parentheses (see PARENTHESIZED_FORMATS), one value must be 
    supplied for each repetition of the specifications enclosed. 
    Consequently, the difference between the following two examples should be 
    noted: 
        7,A,B,C and 3FN.2    and    7,A,7,B,7,C and 3(FN.2) 
    are equivalent to 
        A,B,C and 3F7.2      and    A,B,C and 3F7.2 
6.  In the above example, it was noted that in the specification 3FN.2, one 
    value of N is required, regardless of the value of the repeat count; 
    whereas, the 3(FN.2), the number of values required for N is equal to the 
    repeat count.  The same rule can be extended to include repeat counts 
    whose values are zero. 
    a.  When the repeat count (r) of a single specification is replaced by N 
        and its value is zero, any Ns appearing in that specification must be 
        supplied.  For example, the following combination of list and FORMAT 
            0,4,Y    and    NG20.N,F8.4 
        are equivalent to 
            Y        and    F8.4 
    b.  However, when the repeat count of a parenthesized group is replaced by 
        N and its value is zero, all the specifications appearing within the 
        parentheses are bypassed, including any Ns that may appear.  Thus, 
            O,Y      and    N(G20.N),F8.4 
        are equivalent to 
            Y        and    F8.4 
    In both examples above, no value is supplied for the G specification; 
    however, enclosing the specification within parentheses can be used to 
    determine whether or not the value of N will be supplied. 
    The ability to specify zero repeat counts in this way gives the programmer 
    the facility of selecting or skipping certain specifications within a 
    FORMAT statement.  For example, 
           T=0 
           F=1 
           IF (BOOLE)T=1;F=0 
           PRINT 17,BOOLE,T,F 
        17 FORMAT(L1,N(3HRUE),N$ALSE$) 
    outputs the strings TRUE or FALSE depending on the value of BOOLE.  Note 
    that although an N cannot replace the n in an H specification, the form 
    shown in the above FORMAT statement can be used. 
7.  The value of N may be supplied by an expression in either an input list or 
    an output list, but an expression used for this purpose in an input list 
    is not considered to be a true input list item. 
As an example of the flexibility provided by adjustable format specifications, 
consider the statements: 
        READ (101,205)K,K,(A(J),J=1,K),CODE 
    205 FORMAT(I,NE,A4) 
The value input for K defines not only the number of values to be input into 
the array A, but also the number of conversions to be performed by the E 
specification.  At the same time, the alphanumeric value of CODE can be 
contiguous to the last field input into A, regardless of the number of such 
fields.  Thus, all the following input records can be correctly processed by 
the above statements: 
    1,67.49,HOPA 
    5 -14.3 37 .09711623  0  3E12  JASU 
    ,NONE 
This example illustrates not only adjustable format specifications, but also 
widthless formats and comma field termination. 
READ_ALTERNATE_DIRECT_ACCESS
WRITE_ALTERNATE_DIRECT_ACCESS
NAMELIST_ALTERNATE_STATEMENT
If e is not specified in a RETURN statement, or if the value of e is less than 
one or greater than the number of asterisks in the SUBROUTINE or subroutine 
ENTRY statement that specified the currently referenced name, control returns 
to the CALL statement that initiated the subprogram reference, and this 
completes the execution of the CALL statement. 
If 1< |e| < n (where n is the number of asterisks in the SUBROUTINE or 
subroutine ENTRY statement that specified the currently referenced name), the 
value of e identifies the eth asterisk in the dummy argument list.  Control is 
returned to the statement identified by the alternate return specifier in the 
CALL statement that is associated with the eth asterisk in the dummy argument 
list of the currently referenced name.  This completes the execution of the 
CALL statement. 
Dummy arguments provide a means of passing information between a subprogram 
and the program that called it.  Both FUNCTION and SUBROUTINE subprograms may 
have dummy arguments.  Dummies are merely "formal" parameters, and are used to 
indicate the type, number, and sequence of subprogram arguments.  Since a 
dummy argument does not actually exist, no storage is reserved for it; it is 
only a name used to identify an actual argument in the calling program.  An 
actual argument may be any one of the following: 
    a variable 
    a character substring 
    an array element 
    an array name (i.e., unsubscripted) 
    an expression 
    a statement label 
    a character constant 
    a literal constant 
    a subprogram name 
A dummy itself may be classified within the subprogram as one of the 
following: 
    a variable 
    an array 
    a subprogram 
The following table indicates the permissible correspondence between an 
argument and a dummy: 
  Argument                         Variable   Dummy Array   Subprogram 
Variable or array element 
or character substring             Yes           Yes~            No 
Expression                         Yes           No              No 
Statement label                    Yes           No              No 
Array name                         Yes~          Yes             No 
Character constant                 Yes~          Yes~            No 
Literal constant                   Yes~          Yes~            No 
Subprogram name                    No            No              Yes 
~ A correspondence of this kind may not be entirely meaningful. 
meaningful.  (See DUMMY_ARRAYS). 
An arithmetic expression is a sequence of integer, real, double precision, 
complex, and/or double precision complex elements connected by arithmetic 
operators.  An element in an expression is an operand or a subexpression. 
The arithmetic operators are: 
    Operator       Operation 
    --------       --------- 
     +             Addition (binary) or Positive (unary) 
     -              Subtraction (binary) or Negative (unary) 
     *              Multiplication 
     /              Division 
     **             Exponentiation 
Examples: 
Arithmetic expressions may be of a relatively simple form such as: 
    A 
    -TERM 
    1.2607 
    ACE-DEUCE 
    OML*DE+W9C/XA9RU 
    F(5.8E2)-A1/B9J(L) 
or of a more complicated form as in: 
    X+((12*(G)**L(3)+N/SDS)-(H)+ABC) 
    -B+SQRT(B**2-4*A*C)+T*(S+B/1*(K(J)/(V1-V0)+Z1-Z0)) 
    (X+Y)**3+0.73529865E-7 
    -((M+N)*(Z-Q(J))) 
00001
IF_ARITHMETIC
EXPRESSIONS
ARITHMETIC OVERFLOW: 
Arithmetic values are not supposed to exceed the upper and lower bounds for 
data of their type.  What happens when a value does overflow or underflow 
depends on both its type and the context in which this occurs. 
1.  Overflow/underflow constants appearing in source statements are diagnosed 
    as errors.  Integers that are too large are then converted to 
    floating-point, which will often result in the correct values being 
    computed.  Floating-point overflow values have a maximum value substituted 
    for them.  Floating-point underflow values have a value of zero 
    substituted for them. 
2.  Input values (at run time) are also diagnosed, and a maximum value (for 
    overflow conditions) or zero (for underflow conditions) is substituted. 
3.  Integer overflow resulting from computations (at run time) is ignored. 
    The Bull hardware truncates the result to the low-order 36 bits. 
4.  Integer overflow resulting from computation at compile time is diagnosed 
    with a warning, and that maximum integer is assumed. 
5.  Floating overflow by hardware arithmetic operations (+ - * /) results in a 
    maximum value (with the proper sign), and the floating overflow condition 
    is set ON.  This condition can be tested by using the library routine 
    OVERFL.  (See OVERFL_DVCHK.) 
6.  Floating underflow by arithmetic operations results in substitution of a 
    zero value, and the floating overflow condition is not affected. 
7.  Floating overflow or underflow by mathematical functions (including 
    exponentiation) causes the same value substitution and treating of the 
    floating overflow condition as above; in addition, a diagnostic is 
    produced. 
An array is a set of data.  The data comprising the array form an ordered set. 
Associated with an array is the property of dimension.  FORTRAN-77 arrays may 
have any number of dimensions, with both upper and lower bounds declared.  Any 
array is represented by an identifier whose type is the type of the data.  For 
additional information on arrays see ARRAY_DECLARATORS. 
An array element is a member of the set of data comprising an array.  An array 
element is referenced by the array identifier, followed by a list of 
subscripts enclosed in parentheses.  The format of an array element reference 
is 
v(s1,s2,...,sn) 
where 
v    is the array name. 
si    is a subscript expression (see below). 
n    is the number of subscripts, which must be equal to the number of 
dimensions of the array. 
A subscript may be any expression of integer, real, double precision, complex 
or double precision complex mode.  Non-integer values used as subscripts are 
truncated to integer values. 
    Array       Subscripts        Array Elements 
    -----       ----------        -------------- 
    MATRIX      (3,9)             MATRIX (3,9) 
    CUBE        (J*4,P,3)         CUBE (J*4,P,3) 
    DATA        (Q/I+U-M)         DATA (Q/I+U-M) 
    PARAM       (-N,-8.0,L,A)     PARAM (-N,-8.0,L,A) 
    J35Z        (75,200)          J35Z (75,200) 
Subscripts may themselves contain array element references to any level. 
    ALPHA (I(J))       BETA (M,P(M),H(B)) 
    I(I(I(I(K))))      SIGMA7(130,P(SIGMA7(1,LL))) 
    X(I(M),J(M),K(M))  S7(L*M(A11(S7)+T(K(L)))) 
00002
00001
An array block designator is similar to an array name in that it represents a 
series of array elements.  However, an array block designator denotes a 
specific set of array elements whereas the name denotes all the elements of 
the array. 
The form of an array block designator is 
A1:A2 or A1: or :A2 
where 
A1 and A2    are array elements of the same array name and the subscript value 
of A1 is less than or equal to the subscript value of A2 (if not, the array 
block designator is in error, and it represents no I/O list elements). 
The form A1:A2 represents the sequence of the elements of the array beginning 
with A1 up to and including A2.  For example, with the declaration 
    REAL A(10),B(3,3) 
the following two I/O lists are equivalent: 
    A(2):A(5),B(2,2):B(3,3) 
    A(2),A(3),A(4),A(5),B(2,2),B(3,2),B(1,3),B(2,3),B(3,3) 
If the form :A2 is used, A1 is assumed to be the first element of the array; 
if the form A1: is used, A2 is assumed to be the last element of the array. 
Syntax: 
v(d1,d2,d3,...,dn) 
Parameters: 
v    is the identifier of the array. 
n    is the number of dimensions associated with the array. 
di    defines the range of the corresponding dimension. 
    Each di may take the form 
    ru or rl:ru 
    where 
    ru    is an integer value that defines the upper bound of the dimension 
    range.  For a dummy array that is passed as an argument of a function or 
    subroutine, the upper bound of the last dimension of the array may be 
    specified as an asterisk (*). 
    rl    is an integer value that defines the lower bound of the dimension 
    range.  If rl is not specified, a value of one (1) is assumed. 
The integer value of ru and rl may be a signed or unsigned integer constant, 
or the symbolic name of an integer constant (see PARAMETER_STATEMENT), or a 
constant expression of type integer in which all operands are constants 
(function references and array elements are not permitted), or an adjustable 
limited integer expression in which each operand is either a constant, or a 
dummy variable, or a variable in common. 
An adjustable limited integer expression is permissible only when the 
identifier v is a dummy argument.  (See DUMMY_ARRAY_DIMENSIONS.)  A limited 
integer expression may not contain exponents. 
Description: 
An array declarator declares an identifier to be the name of an array. 
Examples: 
    X(10) 
    ARRAY(5,15,10) 
    PLANE(25:50,0:25) 
    CUBE(-10:1,5,32) 
    LINE(14000) 
    HYPER(3:10,12,-5:5,7) 
    Y(-N:N) 
    Z(I,J,K:L) 
    Q(I*J,K:L) 
Usage Notes: 
An asterisk may be used as the last dimension in an array declarator: 
    Q(I,J,K:*) 
Array declarators may appear in 
    DIMENSION statements 
    Explicit type statements 
    COMMON statements 
    GLOBAL statements 
    VIRTUAL statements 
Note that array declarators with adjustable integer expressions are not 
permitted in COMMON, GLOBAL, or VIRTUAL statements.  Also note that array 
declarators for large virtual arrays must appear in the VIRTUAL statement, or 
in an appropriate type or DIMENSION statement that appears after the VIRTUAL 
statement. 
00002
00003
00001
00004
Although an array may have several dimensions, it is a linear string with 
respect to storage allocation.  This string contains the array elements in 
sequence (from low-address storage toward high-address storage) so that the 
leftmost dimension varies with the highest frequency, the next leftmost 
dimension with the next highest frequency, and so forth. 
A reference to an array element must contain the number of subscripts 
corresponding to the number of dimensions declared for the array (except as 
discussed for EQUIVALENCE statements).  References containing an incorrect 
number of subscripts are treated as errors. 
Furthermore, the value of each subscript should be within the range of the 
corresponding dimension, as specified in the array declarator.  Otherwise, the 
reference may not be to data belonging to the set of elements that comprise 
the array. 
The following figure shows examples of array storage with the linear strings 
converted to columns for pictorial purposes. 
        Array A(3,3,2)       Array B(0:2,-3:2) 
        --------------       ----------------- 
     Item        Element     Item        Element 
     ----        -------     -----       ------- 
       1         A(1,1,1)      1         B(0,-3) 
       2         A(2,1,1)      2         B(1,-3) 
       3         A(3,1,1)      3         B(2,-3) 
       4         A(1,2,1)      4         B(0,-2) 
       5         A(2,2,1)      5         B(1,-2) 
       6         A(3,2,1)      6         B(2,-2) 
       7         A(1,3,1)      7         B(0,-1) 
       8         A(2,3,1)      8         B(1,-1) 
       9         A(3,3,1)      9         B(2,-1) 
      10         A(1,1,2)     10         B(0,0) 
      11         A(2,1,2)     11         B(1,0) 
      12         A(3,1,2)     12         B(2,0) 
      13         A(1,2,2)     13         B(0,1) 
      14         A(2,2,2)     14         B(1,1) 
      15         A(3,2,2)     15         B(2,1) 
      16         A(1,3,2)     16         B(0,2) 
      17         A(2,3,2)     17         B(1,2) 
      18         A(3,3,2)     18         B(2,2) 
00001
00001
00001
00001
STATEMENT_LABEL_ASSIGNMENT
GO_TO_ASSIGNED
    The assignment statement defines a computation to be performed and is used 
    in a manner similar to equations in normal mathematical notation. 
    Assignment statements include the following: 
              SIMPLE_ASSIGNMENT 
              EXTENDED_ASSIGNMENT 
EXTENDED_ASSIGNMENT
SIMPLE_ASSIGNMENT
STATEMENT_LABEL_ASSIGNMENT
STATEMENT_LABEL_ASSIGNMENT
FORTRAN-77 provides the facility of specifying that a program is to be 
compiled with double precision accuracy, without the necessity of manually 
declaring all the variables and changing the constants and the library 
function names.  Automatic double precision causes all real operations to be 
done in double precision, and all complex operations, in double precision 
complex.  When the ADP mode is specified the following changes are made by the 
compiler: 
1.  All identifiers, constants, and user-defined functions that would 
    otherwise be of type real or complex are made double precision and double 
    precision complex, respectively.  This specification is necessary for 
    constants that stand alone, as in the statements: 
        X=.3 
        Z=FUNC(.3) 
2.  Intrinsic library functions that would otherwise return real or complex 
    results are made to return double precision and double precision complex 
    results, respectively.  For example, in the statements 
        INTRINSIC TANH,FLOAT,CATAN 
        X=TANH(Y)+FLOAT(5000000+J) 
        C=CATAN(C2) 
    the functions TANH and FLOAT would return double precision results, while 
    CATAN will return a double precision complex result.  Without the ADP 
    option, the user will have to manually change these function names to 
    DTANH, DFLOAT, and DCATAN. 
This option is recommended primarily as a temporary measure, rather than as a 
standard mode of operation, to provide compatibility with other systems whose 
single precision accuracy is greater than the 36-bit word length of the Bull 
(formerly Honeywell) computer. 
Caution is advised in several areas involving the use of this option: 
1.  When any one program is compiled in the Automatic Double Precision mode, 
    all programs with which it interfaces should also be compiled in this 
    mode, so that arguments will be of the same type and COMMON will be 
    correctly allocated. 
2.  Although intrinsic functions correctly compensate for Automatic Double 
    Precision, external functions may not.  For example, as an intrinsic, the 
    reference ALOG(X) will produce a call on the double precision logarithm 
    routine.  If ALOG is declared EXTERNAL, however, the compiler will indeed 
    classify the function as double precision (in effect, there is no such 
    thing as single precision when operating in this mode), but the external 
    closed routine ALOG supplied by the user might not accept a double 
    precision argument nor return a double precision result.  Therefore, it is 
    necessary for the user to provide external versions that perform the 
    desired calculation. 
3.  Changing a program to double precision can cause allocation diagnostics if 
    COMMON happens to be arranged in such a way that doubleword quantities now 
    fall in odd locations (see STORAGE_ALLOCATION_RULES 
    DOUBLEWORD_VARIABLE_RULES).  However, the compiler will shift COMMON where 
    necessary to compensate for this, and it will usually do so the same way 
    in all subprograms.  The program will usually run correctly despite the 
    diagnostics. 
Auxiliary input/output statements include OPEN, CLOSE, and INQUIRE.  An OPEN 
statement may be used to connect an existing file to a logical unit, or to 
create and connect a file to a logical unit.  A CLOSE statement may be used to 
disconnect a file from a unit.  An INQUIRE statement may be used to inquire 
about attributes of a named file, or of the file connected to a particular 
unit. 
Syntax: 
BACKSPACE {e|(u)|(u,list)} 
Parameters: 
e    is an integer, real, or double precision expression whose value is 
truncated to an integer value. 
u    is an integer, real, or double precision expression or the positional 
form of the unit specifier, UNIT=e. 
list    is any combination of 
        COUNT=n 
        ERR=s 
        IOSTAT=ios 
    where 
    n    is an integer, real, or double precision expression. 
    s    is a statement label. 
    ios    is an integer variable or array ID that is set to an error code 
    (see VALUES_OF_IOS) if an error occurs during the BACKSPACE operation. 
    Otherwise, it is set to zero. 
If both the COUNT=n and ERR=s appear, they may appear in any order and must be 
separated by a comma. 
The logical unit must be connected to a sequential file. 
Description: 
When a BACKSPACE statement is executed, the unit referenced is backspaced one 
(or n) logical records.  Note that an endfile record is considered one record. 
REWIND and BACKSPACE statements executed for tapes already positioned at "load 
point" have no effect. 
00002
00001
BACKSPACE
Statements may contain blanks to improve readability but not within literal 
fields (e.g., in character constants and in FORMAT statements). 
A section of the COMMON region is assigned to blank (or unlabeled) COMMON. 
There is only one such area, and empty block name specifications always refer 
to it.  Furthermore, as opposed to labeled COMMON, blank COMMON areas, defined 
in the various programs and subprograms that comprise an executable program do 
not have to correspond in size.  For instance, the following two subprograms 
define blank COMMON areas of different sizes, and yet both may be portions of 
the same executable program: 
    SUBROUTINE GAMMA              SUBROUTINE ETA 
    COMMON E,D(-100:100),S        COMMON R(10),N(-5:-1) 
      .                             . 
      .                             . 
      .                             . 
where the subroutine GAMMA defines a minimum of 203 words in blank COMMON, 
while the subroutine ETA declares blank COMMON that contains a maximum of 
sixty words, depending on the types of the variables E, D, S, R, and N. 
References may be made to blank COMMON any number of times within a program. 
The multiple references may occur in a single COMMON statement or in several 
COMMON statements.  In either case, all variables defined as being in blank 
COMMON will be placed together in the blank COMMON area. 
Variables in blank COMMON, unlike those in labeled COMMON, may not be 
initialized.  (See DATA_STATEMENT.) 
Syntax: 
BLOCK DATA [sub] 
Parameters: 
sub    is the symbol name of the block data subprogram. 
Description: 
BLOCK DATA statements may contain only declaration statements.  The subprogram 
must be terminated with an END statement and may not be internal to any other 
program.  Since BLOCK DATA subprograms may not be called by other programs, 
they have no names, nor are they executed in the usual sense. 
Examples: 
    BLOCK DATA 
    COMMON /BLK1/A,B(5,5),C,D 
    GLOBAL X,Y,Z 
    COMPLEX Y,A 
    DATA (B(I,J),(I=1,5),J=1,5),Y,C/25*0,(4,32.4),9300/ 
    END 
Note that when initializing variables in labeled COMMON, complete declarations 
should be included for all of the variables in each COMMON block, so that 
1.  The position within the block of those variables that are being 
    initialized is correctly established. 
2.  The size of each COMMON block corresponds to the size declared in all 
    other programs that use it. 
00002
00003
00001
Many FORTRAN systems permit variables in labeled COMMON to be initialized 
(with a DATA statement) only in a special program called a BLOCK DATA 
subprogram.  In FORTRAN-77, both GLOBAL variables and variables in labeled 
COMMON may be initialized in any program unit, exactly like local variables. 
If desired, however, this initialization may be done using a BLOCK DATA 
subprogram, which begins with a BLOCK DATA Statement. 
IF_BLOCK
In some applications, it is desirable to perform BOOLEAN or bit string 
operations on all bits in a word.  The logical operators and constants deal 
only with the values "true" and "false".  Therefore, three features are 
provided to enable full-word bit manipulation: 
1.  Hexadecimal and octal constants.  These are normally of integer type and 
    may be used as actual arguments as well as in DATA statements. 
2.  The intrinsic functions IAND, IOR, IEOR, ICOMPL, IEXCLR, and INOT.  The 
    first three functions accept any number of integer arguments and perform 
    logical AND, OR, and EOR operations on them.  The INOT function accepts 
    one integer argument and returns the 1's complement of the argument. 
    Examples: 
        J=IOR(IAND(K,120777777770000),IRA) 
        MASK=IEOR(N,IOR(LAST,J,INOT(M))) 
        IF (IAND(NAME,120000000777000) .EQ. 120000000640000) 
        GO TO 3 
3.  The intrinsic "shift" functions ISL (Integer Shift Logical), ISA (Integer 
    Shift Arithmetic), and ISC (Integer Shift Circular).  Each of these 
    functions accepts two integer arguments:  the first, the value to be 
    shifted, and the second, the shift count.  This count works the same way 
    as on the 36-bit word length Bull (formerly Honeywell) hardware; namely, a 
    positive count means shift left, a negative count means shift right.  Thus 
    ISL (K,9) shifts K left one byte, while ISC (K,18) interchanges the first 
    halfword with the second. 
These features should only be used in programs that are not expected to be 
compatible with any other machine.  Not only do they take advantage of a 
particular word format, but also there is very little consistency in the 
industry about how (or whether) the Boolean operations are to be specified. 
Since no "Boolean" type is provided, integer quantities should be used.  The 
Boolean functions return integer values; hexadecimal constants as actual 
arguments are interpreted as integers. 
BUFFIN_BUFFERIN
BUFFOUT_BUFFEROUT
Syntax: 
CALL BUFFIN(u,m,s,w) 
      or 
CALL BUFFERIN(u,m,s,w,i[,n][,k]) 
Parameters: 
u    is an integer expression that specifies the logical unit number of the 
device on which the operation is to be performed. 
m    is an integer expression that determines the mode of the operation; if 
m=0, the mode is ASCII; otherwise, the mode is binary.  (An integer 0 or 1 is 
customary.) 
s    indicates the starting location of the internal buffer.  Normally, s is 
the identifier of an array, but it may also be a variable or an array element. 
It may be of any type, although integer is recommended for ease of 
manipulation. 
w    specifies the number of words to be input or output, starting at s, and 
must be an integer expression of positive value. 
i    is an integer variable into which an indication of the status of the 
operation is stored.  The status is indicated as follows: 
    1.  Operation incomplete (only possible with BUFFIN/BUFFOUT). 
    2.  Successful completion; no errors. 
    3.  End-of-file, end-of-tape, or other abnormal condition encountered. 
    4.  Operation complete but read error, write error, or other error 
        condition occurred. 
n    is optional, but when specified is an integer variable into which is 
stored the number of words actually input or output.  In general, this count 
is significant only for input operations; for output operations, n is normally 
equal to w.  If lost data occurs, this count will be greater than w. 
k    is optional, but when specified is an integer variable into which is 
stored the abnormal or error code, if any, as supplied by the monitor. 
Abnormal corresponds to the status, i, having a value of 3; error corresponds 
to i=4.  Examining k allows the user to provide special handling for 
particular abnormal or error conditions.  The value of k will be less than 
256.  In case of lost data, k will be set to 7 and the status, i, will still 
have the value 2.  (Lost data is often designated abnormal in non-FORTRAN 
applications.)  For convenience, the following partial table shows values of k 
in decimal in relation to values of i. 
    Status (i)     k      Description 
    ----------     -      ----------- 
         1         1      Incomplete 
         2         0      Normal, no lost data 
         2         7      Normal, with lost data 
         3        10      End-of-data 
         3         6      End-of-file 
         3         2      End-of-tape (unlabeled tape) 
         4         8      Read error 
         4         8      Write error 
Description: 
A call on BUFFIN or BUFFERIN causes data to be read into memory from the 
specified unit, beginning at the location specified, in the specified mode. 
The actual number of words entered into memory is the minimum of w and n. 
That is, if more words are specified by w than are actually present in the 
physical record that is read, only the number of words appearing in the record 
are changed in memory, and that fact will be reflected by the value of n. 
However, if more words are present on the record than are specified by w, the 
extra words are lost, and n is greater than w. 
The error status is indicated when an unrecoverable error occurs.  However, 
the data is read into memory despite the error, and can be used if the 
programmer chooses to ignore the error. 
BUFFIN proceeds in parallel with the program and other input/output 
operations.  However, with BUFFERIN the operation is complete before control 
is returned to the program. 
For BUFFIN, the indicator (i), the count (n), and the code (k) are not 
returned until the user specifically checks their status with a call on 
ICHECK.  (See ICHECK.) 
Examples: 
The following statements could be used to list binary tapes, in hexadecimal, 
ten words per line, preceded by the record length: 
     INTEGER BUFFER(5000) 
  1  CALL BUFFIN(5, 1,BUFFER, 5000) 
  2  GO TO (2, 3, 4, 3),ICHECK(5,J,N) 
  3  M=MIN(N,5000) 
     PRINT 5,M,(BUFFER(K),K=1,M) 
     GO TO 1 
  4  STOP 
  5  FORMAT(/X,$LENGTH=$,I/(X,10Z10)) 
00002
00003
00001
Syntax: 
CALL BUFFOUT (u,m,s,w) 
     or 
CALL BUFFEROUT(u,m,s,w,i[,n][,k] 
Parameters: 
Same as for BUFFIN and BUFFERIN 
Description: 
A call on BUFFOUT or BUFFEROUT causes the specified number of words to be 
written from memory, beginning with the specified location and in the 
specified mode.  If the number of words requested (w) is greater than the 
maximum size (n) of a physical record on the device being used, i.e., w>n, 
then w is set equal to n.  For example, an attempt to write 30 words on a 
card, in ASCII, would result in n being set to 20. 
If the indicator variable (i) has been set to indicate an error, an 
unrecoverable write error has occurred.  The data has, nonetheless, been 
written on the specified device. 
Records written in binary by these routines are exactly the same as those 
produced by an unformatted WRITE statement. 
Note that the output produced in ASCII by these routines is virtually 
identical to that produced by formatted WRITE statement using 'A' format. 
However, this does not include carriage control on tape printed output or any 
restrictions on the size of magnetic tape records. 
For BUFFOUT, the indicator (i), the count (n), and the code (k) are not 
returned until the user checks their status with a call on ICHECK.  (See 
ICHECK for details.) 
00002
00001
Syntax: 
CALL p [(a1,a2,a3,...an)] 
Parameters: 
p    is the identifier of the subroutine. 
ai   is an argument, which may be a constant, a variable, an array element, an 
expression, a statement label, an array name, an intrinsic function name (see 
INTRINSIC_STATEMENT), or a subprogram name (see ARGUMENTS_DUMMIES). 
Description: 
The CALL statement is used to call or transfer control to a SUBROUTINE 
subprogram. 
A SUBROUTINE is similar to a FUNCTION except that it does not necessarily 
return a value and must not be used in an expression. 
Examples: 
    CALL CHECK 
Arguments that are variables, array elements, or arrays may be modified by a 
SUBROUTINE, effectively returning as many results as desired.  For example, 
the following call might be used to invert matrix A (consisting of K rows and 
columns), store the resulting matrix in array B, and set D(J) equal to the 
determinant of B: 
    CALL INVERT(A,K,B,D(J)) 
A complete discussion of the usage and forms of arguments to subprograms is 
contained under ARGUMENTS_DUMMIES. 
A SUBROUTINE name has no type (e.g., real, integer); it merely identifies the 
block of instructions to be executed as a result of the CALL.  Therefore, the 
appearance of a subprogram name in a CALL statement does not cause it to take 
on any implicit type. 
In FORTRAN-77, it is permissible to call a FUNCTION with a CALL statement. 
The value returned by a complex, double precision complex, or character 
function will be placed in some unpredictable location.  For other types, the 
value returned by the function will be lost.  For example, the library 
subprogram SLITET, which is used to test sense lights, may be referenced 
either as a FUNCTION or a SUBROUTINE as shown below: 
    CALL SLITET(N,J) 
    IF (SLITET(N)) GO TO 3 
Other examples of CALL statements are given below.  Note that statement labels 
are identified by a preceding '*'.  (See ARGUMENTS_DUMMIES.) 
       CALL EOFSET(*44, N) 
       CALL RX23A(X**Y-7, 0, SQRT(A*A+B*B)/DIV, TEST) 
    4  CALLINGB (3*FH > 2*TD .AND. PT(PART)) 
       CALL OUT (GAME(*10) .NE. 1) 
       CALL EVALUATOR 
00002
00003
00001
WRITE_DIRECT
WRITE_DIRECT
CALL
                        The new features provided in this release are: 
                        The XREF option. 
                        The XREF option has been added to the FORTRAN 
                        compiler, and works much like it does in the other 
                        CP-6 compilers.  This option cross- references both 
                        labels and/or identifiers.  The user has the ability 
                        to specify that labels or identifiers are not to be 
                        cross-referenced via an XREF sub-option. 
                        8 character Hollerith literals. 
                        FORTRAN now allows statements that deal with 8 
                        character Hollerith literals, over the previous limit 
                        of 4 characters.  This is to facilitate operations on 
                        DOUBLE type variables. 
It should be noted that in character assignment statements, the movement of 
character data from the source field (the right-hand side of the assignment 
statement) is terminated as soon as either the target field is filled, or the 
source field is emptied.  This is illustrated in the following examples. 
For the following: 
    CHARACTER A*5,B(10)*6,C*4,D*6 
    DATA A/'ABCDE'/,B(1)/'123456'/,C/'ZZZZ'/,D/'XXXXXX'/ 
        Expression     Result 
        ----------     ------ 
        B(2) = A//C    B(2) = 'ABCDEZ' 
        A = C//B(1)    A = 'ZZZZ1' 
        D=C            D = 'ZZZZbb' 
        See HELP (FORTRAN) EXPLICIT_TYPE 
The set of characters acceptable to FORTRAN-77 comprises: 
    Upper-case letters:  A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 
                         and the special characters '$' and '_'. 
    Lower-case letters:  a b c d e f g h i j k l m n o p q r s t u v w x y z 
    Digits:  0 1 2 3 4 5 6 7 8 9 
    Special characters:  + - * / = ( ) . : , ; ' blank > < " 
                         & ` { | } ~ tab @ # % ? ! \ 
This character set conforms to the ASCII standard.  The set of special 
characters on the second line is included for the sake of completeness but 
serves no special purpose.  Lower-case letters are accepted only for text; 
they are not acceptable in FORTRAN-77 identifiers. 
A character expression is a sequence of character operands connected by the 
concatenation operator (//). 
A character operand is one of the following: 
                                                   Examples 
                                                   -------- 
  Character constant                               'AB' 
  Character variable                               CHAR 
  Character array element                          CHAR(2) 
  Character variable substring reference           CHAR (1:2) 
  Character array substring reference              CHAR(2)(1:3) 
  Character function reference                     CHARFUN(A,B) 
  Character expression enclosed in parentheses     (CHAR//'AB') 
The concatenation operator is represented by two consecutive slashes:  //. 
Character expressions are evaluated left to right.  The result of a character 
expression is a character expression.  The length of the result is the sum of 
the lengths of all the character operands in the expression. 
Examples: 
    Expression         Result         Length 
    ----------         ------         ------ 
    'ABCD'//'EFGH'     'ABCDEFGH'     8 
    'AB'//'CD'//'EF'   'ABCDEF'       6 
Note that when a character expression, using character concatenation 
operators, is an argument or a relational operand, each operand of the 
expression must be of known length, that is, the length of the operand must 
not be variable during execution (that is, passed length).  For example, the 
character substring A(2:4) is of constant length; the character substrings 
A(I:J) and A(K:K+1) are of known length at compile time if they are not 
dummies with passed length attributes.  That is, the maximum possible legal 
length of A(I:J) is known at compile time. 
The use of the concatenation operator with character variables is illustrated 
in the following example: 
    CHARACTER A*5,B(10)*6,C*4,D*6 
    DATA A/'ABCDE'/,B(1)/'123456'/,C/'ZZZZ'/,D/'XXXXXX'/ 
        Expression               Result 
        ----------               ------ 
        A(2:3)//B(1)//'ABC'      'BC123456ABC' 
It is often useful to deal with alphanumeric characters in a FORTRAN program. 
Character variables and character constants as well as Hollerith and literal 
constants are provided for this purpose, as are the A, R, and H format 
specifications.  Normally, variables and constants containing character 
strings should not be mixed with those containing numeric values; that is, 
characters should not be used as if they were numeric values, or vice versa. 
A FORTRAN program that depends on the numeric representations of characters 
loses compatibility with almost all other computer systems, as these 
representations vary widely.  For example, the character "A" has at least the 
following numeric values: 
    Decimal Value         Machine 
    -------------         ------- 
       65                 Bull, L66 (ASCII) 
      193                 Xerox Sigma, IBM System 360 (EBCDIC) 
       17                 Xerox 92/900/9300, IBM 7000 Series, CDC 3000 Series 
       41                 IBM 1620 
       61                 IBM 7070 
       49                 CDC 160, DDP 24 
        6                 Univac 1107 
       33                 Xerox 940, PDP 6 
        1                 Bendix G 20, CDC 6000 Series 
       57                 LGP 30 
       10                 Alwac 3-E 
When it is necessary to distinguish one character from another in a FORTRAN 
program, there are several ways to accomplish this without sacrificing 
compatibility between machines. 
1.  If only a few characters need be recognized, they can be distinguished by 
    comparing them with Hollerith constants.  For example: 
            READ(105,12)NAME,X 
      12    FORMAT(A1,F19,0)) 
            IF(NAME .EQ. 1HA) GO TO 41 
            IF(NAME .EQ. 1HZ) GO TO 42 
            etc. 
2.  If all possible characters must be distinguished, it is common practice to 
    use a "jump" table, based on the numeric value of the character.  This has 
    often been done with a computed GO TO.  As pointed out, such a technique 
    is machine dependent.  In CP-6 FORTRAN-77, there is a way to establish 
    such a jump table without depending on a particular set of numeric values. 
    First, the statement labels constituting the jump table can be ASSIGNed to 
    the elements of an array, in a manner such as the following: 
         INTEGER JUMP(0:511) 
         DO 3, I=0,511 
      3  ASSIGN 999 to JUMP(I) 
         ASSIGN 1000 TO JUMP(1RA) 
         ASSIGN 1001 TO JUMP(1RB) 
         . 
         . 
         . 
         ASSIGN 2000 TO JUMP(1R.) 
         ASSIGN 2001 TO JUMP(1R,) 
         ASSIGN 2002 TO JUMP(1R() 
         etc. 
Initially, all elements are ASSIGNed the statement label 999.  Then, the 
meaningful characters are used on subscript values to position the 
corresponding statement labels.  The array can now be referenced as follows: 
     READ(105,14) NAME 
  14 FORMAT(R1) 
     GO TO JUMP(NAME) 
Note that although the numeric values of the characters are indeed used, this 
method will work in the same way regardless of what those values are.  The 
only requirement in the above example is that the character be represented by 
values not exceeding 511 (the dimension of the array JUMP). 
The standard character set for use with CP-6 FORTRAN-77 is ASCII (American 
Standard Code for Information Interchange).  The CP-6 system can be instructed 
to recognize, instead, the character set that was used in CPV (and that is 
still used in some other operating systems) -- EBCDIC (Extended 
Binary-Coded-Decimal Interchange Code).  This contains: 
o   The CP-6 ASCII Code. 
o   The CP-6 EBCDIC Code 
o   A discussion of character manipulation using numeric data. 
o   The CP-6 FORTRAN-77 collating sequence 
Note that the Bull 36-bit word-length hardware internal hexadecimal or ASCII 
representation (in memory) of every character is the same as the magnetic tape 
representation.  However, it is recommended that the FORTRAN programmer not 
take advantage of these numeric representations.  See CHARACTER_MANIPULATION. 
    A character substring name is used to represent a portion of a character 
    variable or character array element.  It is of one of the following forms: 
    For Character 
    Variable             For Character Array Element 
    --------             --------------------------- 
    V(P1:P2)             A(S1,S2,...,Sn)(P1:P2) 
    V(P1:)               A(S1,S2,...,Sn)(P1:) 
    V(:P2)               A(S1,S2,...,Sn)(:P2) 
    V(:)                 A(S1,S2,...,Sn)(:) 
where 
P1    is the beginning character position of the portion of the character 
string and if omitted, is assumed to be 1. 
P2    is the terminal character position of the portion of the character 
string, and if omitted is assumed to be len, when len is the declared length. 
Si    are subscript expressions and n is the number of dimensions of A. 
Let len be the declared length of the character identifier, then the following 
relationship must be true: 
     1 < P1 < P2 <  len = len (the character identifier) 
    In a program unit, an identifier may be classified as referring to an 
    entity in any of the following categories: 
        variable 
        array 
        subprogram 
        COMMON block 
    The category into which an identifier is placed and the type (if any) 
    associated with it depend on the contexts in which the identifier appears 
    in the program.  These appearances constitute the explicit and implicit 
    declaration of the way the identifier is to be classified.  (See 
    IDENTIFIERS) 
CLASSIFICATION_OF_IDENTIFIERS
Syntax: 
CLOSE([UNIT=]u[,STATUS=dis][,ERR=s][,IOSTAT=ios]) 
Parameters: 
u    is an external unit specifier that specifies the unit to be closed.  It 
may be the positional or the keyword form. 
dis    is a character expression that determines the disposition of the file 
that is connected to the specified unit.  It must have a value of 'KEEP', 
'SAVE', or 'DELETE'.  If 'KEEP' or 'SAVE' is specified, the file continues to 
exist after the execution of the CLOSE statement, unless the file type is 
'SCRATCH', in which case the file is deleted.  If 'DELETE' is specified, the 
file ceases to exist after execution of the CLOSE statement.  If this 
specifier is omitted and the file's type is SCRATCH, the assumed value is 
DELETE.  With any other file type, the assumed value is 'KEEP'. 
s    is a statement label for the error specifier. 
ios    is an integer variable or array ID.  Execution of a CLOSE statement 
causes ios to be set to a zero value if no error condition exists.  If an 
error occurs during the CLOSE operation, the location ios will be set to an 
error code.  (See VALUES_OF_IOS) 
Description: 
Used to disconnect a file from a unit.  The closing of a unit may occur in any 
program unit of an executable program and need not be done in the same program 
unit as the opening.  The closing of a unit that has no file connected to it 
is permitted and has no effect. 
After a unit has been disconnected by a CLOSE statement, it may be reconnected 
within the same executable program either to the same file or a different 
file. 
00002
00001
CLOSE
The following table shows the Cp-6 FORTRAN-77 collating sequence. 
  Character   ASCII Code   Character   ASCII Code   Character   ASCII Code 
              (Octal)                  (Octal)                  (Octal) 
    Blank        40            7          67            N        116 
      !          41            8          70            O        117 
      "          42            9          71            P        120 
      #          43            :          72            Q        121 
      $          44            ;          73            R        122 
      %          45            <          74            S        123 
      &          46            =          75            T        124 
      '          47            >          76            U        125 
      (          50            ?          77            V        126 
      )          51            @         100            W        127 
      *          52            A         101            X        130 
      +          53            B         102            Y        131 
      ,          54            C         103            Z        132 
      -          55            D         104            [        133 
      .          56            E         105            /        134 
      /          57            F         106            ]        135 
      0          60            G         107            ^        136 
      1          61            H         110            -        137 
      2          62            I         111            `        140 
      3          63            J         112       a thru z 141 thru 172 
      4          64            K         113            {        173 
      5          65            L         114            |        174 
      6          66            M         115            }        175 
                                                        ~        176 
    The characters listed below are processed as indicated when they appear in 
    column 1 of a Bull FORTRAN-77 initial line.  All other letters and special 
    characters (as opposed to digits) cause an error if they appear in column 
    1 of an initial line.  Note that on a continuation line, all characters 
    are ignored in column 1, except the character C or the character *. 
  Character     Description 
  ---------     ----------- 
  C, *          Comment line.  May appear anywhere in a program except 
                immediately preceding continuation lines. 
  X,Y,Z         Conditional compilation.  Normally treated as a comment line. 
                However, if the X option is selected on the !FORTRAN control 
                card, the rest of the line is processed as if the X were not 
                present.  (See CONDITIONAL_COMPILATION.) 
  !             Bull CP-6 IBEX command control card.  These cards are 
                processed by CP-6 operating system. 
  /             Treated as comment lines.  These characters are used as 
  "             control characters in other FORTRAN systems. 
  I or D        Error.  To perform IBM 7090 FORTRAN II double precision and 
                complex operations use COMPLEX or DOUBLE PRECISION statements 
                (and remove the D or I from column 1). 
  B             Error.  For Boolean operations use the Boolean library 
                functions.  The functions IAND, IOR, IEOR and INOT should not 
                be used. 
Column 1 may contain either the character C or * to indicate that the line is 
to be treated as comment, with no effect on the program.  Comment lines may 
appear anywhere in the program.  See also:  EMBEDDED_COMMENTS 
COMMENTS
COMMON_STATEMENT
BLANK_COMMON
Each labeled COMMON block and the blank COMMON area contain, in order of their 
appearance, the elements declared to be in the labeled block or the unlabeled 
area.  The elements in each section of the COMMON region are arranged from low 
address storage towards high address storage.  That is, the first element to 
be declared as being in a particular section is contained in the low address 
word or words of that section, while the last element to be declared as being 
in the section is contained in the high address word or words of the section. 
Arrays are stored in their normal sequence within the COMMON block or area. 
(See ARRAY_STORAGE.)  For example, the statements 
    COMMON /E/W,X(3,3)//T,B,Q/E/J 
    COMMON K,M/E/Y//C(0:3),H,N(2),Z 
cause the following arrangement of COMMON: 
    Item     Block E     Blank COMMON 
    ----     -------     ------------ 
       1     W               T 
       2     X(1,1)          B 
       3     X(2,1)          Q 
       4     X(3,1)          K 
       5     X(1,2)          M 
       6     X(2,2)          C(0) 
       7     X(3,2)          C(1) 
       8     X(1,3)          C(2) 
       9     X(2,3)          C(3) 
      10     X(3,3)          H 
      11     J               N(1) 
      12     Y               N(2) 
      13                     Z 
Note that, since a segment of the COMMON region may be defined differently in 
each program, it may be quite important to be aware of which items in a 
segment contain certain variables.  For example, 
    SUBROUTINE TOM       SUBROUTINE DICK        SUBROUTINE HARRY 
    COMMON /S/A,B(0:100) COMMON /S/A,X(51)      COMMON /S/ALPHA(52) 
                         COMMON /S/Y(50)        COMMON /S/Y(50) 
    .                    .                      . 
    .                    .                      . 
    .                    .                      . 
will define the block S as follows: 
    Item     TOM      DICK     HARRY 
    ----     ---      ----     ----- 
       1     A        A        ALPHA 
       2     B(0)     X(1)     ALPHA(2) 
       3     B(1)     X(2)     ALPHA(3) 
       .     .        .        . 
       .     .        .        . 
       .     .        .        . 
      52     B(50)    X(51)    ALPHA(52) 
      53     B(51)    Y(1)     Y(1) 
      54     B(52)    Y(2)     Y(2) 
       .     .        .        . 
       .     .        .        . 
       .     .        .        . 
     102     B(100)   Y(50)    Y(50) 
which allows the routines TOM and DICK to access the variable A by that 
identifier, the routines DICK and HARRY to access the array variable Y by that 
identifier, all the while maintaining the integrity of the block S.  (These 
examples assume A, B, X, Y, and ALPHA are  of the same type.) 
Note the arrangement of storage in the following examples, which contain data 
of various types: 
    SUBROUTINE ABLE      SUBROUTINE BAKER 
    INTEGER M,N(10)      COMPLEX C,X 
    REAL T,V             LOGICAL L,B 
    COMMON //M,T,N,V     COMMON L,X,C,B(-5:0) 
    .                    . 
    .                    . 
    .                    . 
where blank COMMON is arranged as follows: 
    OFFSET      Word     ABLE     BAKER 
    ------      ----     ----     ----- 
       0           1     M        L 
       1           2     T        X1 
       2           3     N(1)     X2 
       3           4     N(2)     C1 
       4           5     N(3)     C2 
       5           6     N(4)     B(-5) 
       6           7     N(5)     B(-4) 
       7           8     N(6)     B(-3) 
       8           9     N(7)     B(-2) 
       9          10     N(8)     B(-1) 
      10          11     N(9)     B(-0) 
      11          12     N(10) 
      12          13     V 
Note that T and V each require one word of storage, and C and X require two 
words each (as indicated by the subscripts).  In this case, extreme care must 
be used to ensure that the value contained in any one word or words is of the 
type expected by the references to those words. 
The following is a list of common library routines: 
ABCL (provides the clock time) 
ABORTERR (provides ABORTSET error code) 
ACPU (total CPU time) 
ASSIGNM (provides the SIGNM) 
CLK$ (clock time) 
DAT$ (date) 
DBCL (clock time) 
DCPU (total CPU time) 
DVCHKCNT (count of divide checkfaults) 
DRANDOM (double precision random number) 
DRANDRS (reads random number seed) 
DRANDSS (sets random number seed) 
DSIGNM (provides SIGNM) 
IDA (provides date) 
IRANDRS (reads the random number seed) 
IRANDSS (sets the random number seed) 
JUL$ (Julian date) 
MATHERR (math error code) 
OVFLOCNT (overflow count) 
RANDIS (initialize random number seed) 
RANDOM (single precision random number) 
RANDRS (reads random number seed) 
RANDSS (sets random number seed) 
SENSLITE (provides the sense light) 
UNFLOCNT (underflow count) 
To display a particular common library routine 
(such as RANDOM) enter: 
HELP (FORTRAN) COMMON_LIB RANDOM 
To receive a display of all routines, enter ?? 
ABCL 
Syntax: 
CALL ABCL (hours) 
Parameters: 
hours    is a real variable into which the computer clock time is stored as a 
single precision floating point number. 
Description: 
This routine is called by the user to return the clock time.  The output is in 
hours (including fraction) since midnight as a single precision floating point 
number.  The number of milliseconds since midnight is converted to hours and a 
fraction. 
Example: 
    REAL HOURS 
    CALL ABCL(HOURS) 
    WRITE(108,'(2X,F5.2)')HOURS 
    END 
ABORTERR 
Syntax: 
CALL ABORTERR (error) 
Parameters: 
error    is an integer variable into which the error code that occurred while 
ABORTSET was set is returned. 
Description: 
Provides error code that occurred while ABORTSET was set.  This routine is 
called by the user to return the error code as an integer. 
ACPU 
Syntax: 
CALL ACPU (seconds) 
Parameters: 
seconds    is a real variable into which the total CPU time in seconds is 
stored. 
Description: 
This routine is called by the user to return the CPU time.  The output is CPU 
time in seconds (including fraction) as a single precision floating point 
number.  This routine returns a time of -1.0 seconds if an error occurs.  This 
routine returns a time of -1.0 seconds if an error occurs.  The time that will 
be returned to the caller is the total of all the users execute and service 
time for the job. 
Example: 
    REAL SECONDS 
    CALL ACPU(SECONDS) 
    OUTPUT' THE TOTAL CPU TIME IN SECONDS IS' 
    WRITE(108,'(2X,F15.1)')SECONDS 
    END 
ASIGNM 
Syntax: 
CALL ASIGNM (float,result) 
Parameters: 
float    is a single precision floating point number. 
result    is a real variable into which the SIGNUM is stored. 
Description: 
This routine is called by the user to return the sign of the passed parameter 
(float).  The output is a single precision number of the sign of the passed 
value of float: 
   -1.0 if float is negative 
    0.0 if float is zero 
   +1.0 if float is positive 
Example: 
    REAL V1,V2,V3 
    CALL ASIGNM (3.5,V1) 
    WRITE(108,'(2X,F5.1)')V1 
    CALL ASIGNUM(0.0,V2) 
    WRITE(108,'(2X,F5.1)')V2 
    CALL ASIGNM (-14.7,V3) 
    WRITE(108,'(2X,F5.1)')V3 
    END 
CLK$ 
Syntax: 
CALL CLK$ (time) 
Parameters: 
time    is a character variable with a length of eight characters. 
Description: 
Provides the clock time as 'HH:MM:SS'.  The output is a string of eight 
characters.  Of the complete time with hundredths of a second ('HH:MM:SS.HH'), 
the portion 'HH:MM:SS' is returned to the caller. 
Example: 
    CHARACTER*8 TIME 
    DATA TIME/'HH:MM:SS'/ 
    CALL CLK$(TIME) 
    WRITE(108),'(2X,A8)')TIME 
    END 
DAT$ 
Syntax: 
CALL DAT$ (date) 
Parameters: 
date    is a character variable with length of eight characters into which the 
date is stored. 
Description: 
This routine is called by the user to return the date.  The date is output in 
the form of 'MM/DD/YY'. 
Example: 
    CHARACTER*8 DATE 
    DATA DATE/'MM/DD/YY'/ 
    CALL DAT$(DATE) 
    END 
DBCL 
Syntax: 
CALL DBCL (hours) 
Parameters: 
hours    is a double precision variable into which the value time is stored. 
Description: 
This routine is called by the user to return the clock time as a double 
precision floating point number.  The output is in hours (including fraction) 
since midnight as a double precision floating point number. 
Example: 
    DOUBLE PRECISION HOURS 
    CALL DBCL(HOURS) 
    WRITE(108,'(2X,F5.2)')HOURS 
    END 
DCPU 
Syntax: 
CALL DCPU (seconds) 
Parameters: 
seconds    is a double precision variable into which the value seconds is 
returned. 
Description: 
This routine is called by the user to return the CPU time for user execution 
and service used during the current job.  The output is CPU time in seconds 
(including fractions) as a double precision floating point number.  This value 
is the total user execute and service time for the job. 
DVCHKCNT 
Syntax: 
CALL DVCHKCNT (count) 
Parameters: 
count    is an integer variable into which the value count is returned. 
Description: 
This routine provides the count of the number of divide checkfaults that have 
occurred in the user's program during execution.  The output is returned as an 
integer number. 
DRANDOM 
Syntax: 
CALL DRANDOM (dfloat) 
Parameters: 
dfloat    is a double precision variable in which the random number is 
returned as a double precision floating point number. 
Description: 
This routine is called by the user to get a random number as a double 
precision floating point number between 0.0 and 1.0.  The output is a random 
number as a double precision floating point number between 0.0 and 1.0. 
DRANDRS 
Syntax: 
CALL DRANDRS (dfloat) 
Parameters: 
dfloat    is a double precision variable in which the random number seed is 
returned as a double precision floating point number. 
Description: 
This routine is called by the user to read the random number seed.  The output 
is the random number seed as a double precision floating point number. 
DRANDSS 
Syntax: 
CALL DRANDSS (dfloat) 
Parameters: 
dfloat    is a double precision floating point number used to set the random 
number seed. 
Description: 
This routine is called by the user to set the random number seed from an input 
double precision floating point number. 
DSIGNM 
Syntax: 
CALL DSIGNM (float,result) 
Parameters: 
float    double precision floating point number. 
result    is a double precision variable into which the sign of the passed 
value (float) is returned as a double precision floating point number. 
Description: 
This routine is called by the user to return the sign of the passed parameter 
(float).  The output is a double precision number of the sign of the passed 
value of float: 
   -1.0 if float is negative 
    0.0 is float is zero 
   +1.0 is float is positive 
IDA 
Syntax: 
CALL IDA (date) 
Parameters: 
date    is an integer variable into which the value date is returned as a 
decimal integer. 
Description: 
Provides the date as an integer; this routine is called by the user to return 
the date as 'YYMMDD' converted to a decimal integer.  The output is the date 
YYMMDD converted to a decimal integer. 
Example: 
    INTEGER DATE 
    DATA DATE/'YYMMDD'/ 
    CALL IDA(DATE) 
    WRITE(108,'(2X,I6)')DATE 
    END 
Syntax: 
CALL IRANDRS (integer) 
Parameters: 
integer is a full word integer variable into which the value of the random 
number seed is returned. 
Description: 
Reads the random number seed and returns the value to the user as a full word 
integer. 
Syntax: 
CALL IRANDSS (integer) 
Parameters: 
integer is a full word integer variable used by the user to get the random 
number seed. 
Description: 
This routine is called by the user to set the random number seed from an input 
full word integer value. 
JUL$ 
Syntax: 
CALL JUL$ (jul) 
Parameters: 
jul    is a character variable of length 6 into which the Julian date is 
returned. 
Description: 
Provides the Julian day and year; this routine is called by the user to return 
the Julian date as 'YY/DDD'.  The date as 'YYDDD' is obtained and a 
concatenation is performed to build 'YY/DDD'. 
Example: 
    CHARACTER*6 JULIAN 
    DATA JULIAN/'YYDDD'/ 
    CALL JUL$(JULIAN) 
    WRITE(108,'(2X,A6)')JULIAN 
    END 
MATHERR 
Syntax: 
CALL MATHERR (error) 
Parameters: 
error    is an integer variable into which the math error code is returned. 
Description: 
Provides the math error code; this routine is called by the user to return an 
18-bit error code as an integer. 
OVFLOCNT 
Syntax: 
CALL OVFLOCNT (count) 
Parameters: 
count    is an integer variable into which the overflow count is returned. 
Description: 
This routine provides the count of overflows that occurred in the user's 
program during execution. 
RANDIS 
Syntax: 
CALL RANDIS 
Parameters: 
None. 
Description: 
Used to initialize the random number seed from the millisecond clock. 
RANDOM 
Syntax: 
CALL RANDOM (float) 
Parameters: 
float    is a real (or single precision floating point) variable into which a 
random number value is stored. 
Description: 
This routine is called by the user to get a random number as a single 
precision floating point number between 0.0 and 1.0. 
Example: 
    REAL R 
    CALL RANDOM(R) 
    WRITE(108,'(2X,F3.1)')R 
    END 
RANDRS 
Syntax: 
CALL RANDRS (float) 
Parameters: 
float    is a real variable into which the value of the random number seed is 
returned. 
Description: 
Reads the random number seed and returns the value to the user as a single 
precision floating point number. 
RANDSS 
Syntax: 
CALL RANDSS (float) 
Parameters: 
float    is a single precision floating point number used by the user to set 
the random number seed. 
Description: 
This routine is called by the user to set the random number seed from an input 
single precision floating point number. 
SENSLITE 
Syntax: 
CALL SENSLITE (lights) 
Parameters: 
lights    is an integer variable into which the value of all sense lights is 
stored as an integer. 
Description: 
This routine is called by the user to obtain the current status of all sense 
lights.  The value returned is an integer value. 
UNFLOCNT 
Syntax: 
CALL UNFLOCNT (count) 
Parameters: 
count    is an integer variable into which the value of the underflow count is 
returned. 
Description: 
This routine provides the user with the count of underflows that have occurred 
in the user's program during execution. 
00001
00002
00003
00004
00005
00006
00007
00008
00010
00011
00012
00013
00009
00014
00015
00016
00017
00018
00019
00020
00021
00022
00023
Syntax: 
COMMON w1w2w3...wn 
       or 
COMMON w1,w2,w3,...wn 
Parameters: 
each wi    has the form /c/v1,v2...vm 
    where 
    c    is either the identifier of a labeled COMMON block, or is absent 
    indicating blank COMMON. 
    vi    is an element in the COMMON block c and may be a variable, or array 
    name, or array declarator. 
Note that if any datum in the COMMON block is of type character, all data in 
the block must be of type character. 
When w1 (the first specification in the statement) is to specify blank COMMON, 
the slashes may be omitted.  In all other places, blank COMMON is indicated by 
two consecutive slashes. 
Description: 
The COMMON statement is used to assign variables to a region of storage called 
COMMON storage.  COMMON storage provides a means by which more than one 
program or subprogram may reference the same data. 
Examples: 
    COMMON MARKET,SENSE/GROUP3/X,Y,JUMP//GHIA,COLD 
For each specification (wi), the variables listed are assigned to the 
indicated COMMON block or to blank COMMON.  The variables are assigned in the 
order they appear.  Thus, in the above example, MARKET, SENSE, GHIA, and COLD 
are assigned to blank COMMON, while X, Y, and JUMP are placed in the labeled 
COMMON block GROUP3. 
00002
00003
00001
EXPRESSIONS
PRECISION_OPTION
Compilation options control the compilation process, and are placed on the 
!FORTRAN command line.  If more than one program unit is contained in the 
source, the compiler normally processes all of the source until it encounters 
an EOF (end of file). 
For information on a specific compiler option, enter: 
HELP (FORTRAN) COMPILER_OPTIONS option 
For a list of the compiler option names, enter: 
HELP (FORTRAN) OPTION_GROUPS 
To view all options, enter ??. 
In the absence of options in the calling sequence for a compilation, the 
compiler assumes default options (see the topic DEFAULT_OPTIONS). 
Individual FORTRAN-77 compilation options available to the user are listed in 
alphabetical order and described in the following table.  Options in conflict 
with each other can be listed by entering HELP (FORTRAN) COMPILER_OPTIONS 
CONFLICTING_OPTIONS 
  Option             Purpose                                      Class 
ADP                Causes all real operations                   PGC 
                   to be performed in double precision 
                   accuracy, and all complex 
                   operations to be performed in 
                   double complex accuracy. 
ANS                Limits FORTRAN language features             PGC 
                   to those provided by ANS, i.e., 
                   exclusive of Bull FORTRAN 77 
                   extensions.  Applies only to 
                   compile time program checking. 
                   A STEPCC code value and an OU severity 
                   level of 4 will be generated when the 
                   ANS and WARN (default) options are 
                   selected.  When the ANS and NWARN are 
                   used together, the STEPCC and OU 
                   severity level will be set to zero. 
BC or BC(ALL)      The Batch Compile (BC) mode                  IC 
                   enables the user to compile 
                   more than one source program 
                   unit in a single compile, or 
                   to enter and compile several 
                   program units or subroutines 
                   at one time.  BC and BC(ALL) 
                   are the same.  Either specifies: 
                   1)  in interactive use without 
                       a program source file, that 
                       FORTRAN will continue to 
                       prompt for input until an 
                       escape F is entered by the 
                       user; 
                   2)  in interactive use with a 
                       program source file, that 
                       FORTRAN will compile all 
                       program units in the 
                       file; 
                   3)  when used with batch jobs, 
                       that FORTRAN will compile 
                       all program units either 
                       in the job, or in a 
  or                   job-specified file. 
BC(n)              In interactive use, should only              IC 
                   be used with a program source 
                   file.  May also be used with 
                   batch jobs.  Specifies: 
                   1)  in interactive use, that 
                       FORTRAN will compile the 
                       nth program unit in the 
                       file and then exit; 
                   2)  with batch jobs, that 
                       FORTRAN compile the nth 
                       program unit in the job 
                       or in a job-specified 
                       file, then exit. 
  or 
BC(1,2,3,...n)     Specifies: 
                   1)  in interactive use without               IC 
                       a program source file, 
                       that FORTRAN prompt for n 
                       program units and then 
                       exit; 
                   2)  in interactive use with a 
                       program source file, that 
                       FORTRAN select and compile 
                       the indicated program units; 
                   3)  with batch jobs, that 
                       FORTRAN select and compile 
                       the indicated program units 
                       either from the job or from 
                       a job-specified file. 
                   BC(1,2,3,...n) allows a maximum of 
                   eight program units to be selected. 
                   Multiple entries can be in any 
                   order; each entry must be greater 
                   than 0. 
CC(   LBL$COM [,LBL$COM]... [,LBL$COM]   )                     PGC 
                   CC-COMMON in Constant storage 
                   LBL$COMi-the name of the labeled 
                   COMMON block(s) that are to be 
                   allocated in write protected storage. 
                   The allocation of COMMON in a read 
                   only control section is accomplished 
                   by use of the 'CC' compilation option. 
                   Unlabelled COMMON may also be specified 
                   by use of the name F4$COM. 
CDS(L$C[ ,L$C] ... [,L$C])                                     PGC 
                   CDS-Specifies that allocation of 
                   COMMON in Data segments is to be 
                   performed. 
                   L$Ci-is of the form 
                        NAME=SEGNUM 
                        NAME-is the name of the labeled 
                        COMMON block to be allocated 
                        in a data segment(s). 
                        SEGNUM-is the number of the data 
                        segment in which the labeled 
                        COMMON block is to be allocated. 
                        Numbers allowed are 4-8. 
                   The user may allocate as many labeled 
                   common blocks in data segments as will 
                   fit in 1446K words.  The maximum total 
                   common block size available to the user 
                   is 1446K words. 
                   Unlabelled COMMON can also be allocated 
                   in a data segment.  The name to use for 
                   unlabelled COMMON is F4$COM.  That is: 
                   CDS(F4$COM=segnum). 
                   Each labeled COMMON block that is 
                   specified with the FORTRAN compilation 
                   option CDS will be allocated in the 
                   specified data segment.  One of the 
                   advantages of this feature is the 
                   capability of having arrays of up 
                   to 256K in size.  This should satisfy 
                   most user needs for large arrays, and 
                   the resultant procedure code is more 
                   efficient than the code generated for 
                   Virtual Arrays.  The number of common 
                   blocks that can be specified on the 
                   CDS option is limited only by memory 
                   requirements. 
                   Care must be taken when the CDS 
                   is used with overlaid programs. 
                   The labeled common blocks should 
                   be root labeled common blocks. 
CF                 Specifies that the FORMAT statement          PGC 
                   is encoded at compile time.  The 
                   run-time routines do not have to do 
                   any syntax checking or textual format 
                   checking.  This is the default mode 
                   for compilations. 
DBM                Data Base Management option.                 PGC 
                   Allows the mixing of character 
                   and non-character data entities 
                   in a common block. 
DIAG               Does a diagnostic check of                   OC,PGC 
                   the program; no object unit 
                   is produced for the compiled 
                   program. 
DM[AP]             Produces a full summary of                   OC 
                   the program's data. 
DO                 Outputs error messages on                    OC 
                   the LL (listing log) device 
                   when diagnostic output (DO) is 
                   assigned to a device other 
                   than LL. 
DOZ                Allows the use of IBM                        PGC 
                   hexadecimal constants (form 
                   is Zddd), and octal constants 
                   of the same form (that is, 
                   Oddd).  The d's represent 
                   the appropriate hexadecimal 
                   or octal digits. 
DPS8               Specifies that the target                    PGC 
                   machine is not a DPS90, but that 
                   the vector generic/intrinsic 
                   functions/calls are to be 
                   performed by run-time support 
                   routines.  If the compilation machine 
                   is not a DPS90 and the DPS8 is 
                   not selected, the compiler treats 
                   the vector generic/intrinsic 
                   functions/calls as references to 
                   user-supplied functions or user- 
                   supplied subprograms; this is 
                   done to guard the integrity of 
                   pre-D00 FORTRAN source programs. 
DPS9[0[(n)]]       Specifies that the generated OU is           PGC 
                   targeted for execution on a DPS90 
                   machine.  Hence, all of the vector 
                   intrinsic functions/calls (with the 
                   exception of the IVDIV intrinsic) will 
                   be generated in-line to utilize the 
                   vector array instructions. 
                   "n" specifies the maximum loop 
                   iteration count for any of the DO 
                   loops to be optimized.  "n" is 
                   meaningful if the VOP compilation 
                   option is specified explicitly (or 
                   assumed by default during compilations 
                   performed on the DPS 90 machine). 
                   If "n" is omitted, 1024 is assumed. 
                   The compiler reserves a temporary 
                   vector work area based on the 
                   iteration count that is determined 
                   or assumed at compilation time. 
FF                 Allows users to submit                       IC 
                   "free form" source programs for 
                   compilation.  Free form means the 
                   following: 
                   1.  Columns 1 through 6 are no 
                       longer restricted for statement 
                       numbers and continuation. 
                       Statement numbers may appear 
                       anywhere on the input line. 
                       The statement text may appear 
                       anywhere on the input line 
                       following the statement number. 
                   2.  Characters beyond column 72 will 
                       be processed as part of the 
                       source statement text. 
                   3.  Source program input will be in 
                       the form of variable length 
                       records instead of fixed, 
                       80-character records.  Input records 
                       may contain more than 80 characters. 
                   4.  A comment character in column 1 
                       causes the entire input record 
                       to be treated as a comment. 
                   5.  Continuation for non-terminal 
                       input is designated by an & as 
                       the first non-blank character in 
                       an input record.  Continuation for 
                       terminal input is designated by a 
                       colon (:) in the last character 
                       in the input record. 
                       Continuation of X, Y, or Z 
                       conditional compilation lines 
                       for non-terminal input is 
                       designated by the corresponding 
                       X, Y, or Z in column 1, followed 
                       by an & as the first non-blank 
                       character.  For terminal input, 
                       continuation is designated by a 
                       colon (:) as the last character 
                       on the input line. 
                   Restrictions: 
                   Assignment statements whose 
                   first character is a C, X, Y, 
                   or Z, or statements beginning with 
                   the character C must not begin 
                   in column one.  These statements 
                   will be ignored or incorrectly 
                   interpreted by the compiler. 
IMR                Initializes memory randomly                  PGC 
                   (to the universal time stamp). 
IMZ                Initializes memory to                        PGC 
                   zero. 
KAP[(n)]           This option causes the KAI KAP vector        PGC 
                   optimizations to be performed.  This 
                   option is only meaningful if the target 
                   machine has vector hardware instructions 
                   such as the DPS90.  The KAP vector 
                   optimizations are a super set of those 
                   that are performed by the VOP or IAP 
                   vectorizations.  The KAP vectorizing 
                   capability is an optional feature for 
                   Fortran, and is available only if it is 
                   purchased separately. 
                   The optional parameter, n, allows the 
                   user to specify the number of virtual 
                   buffer pages that are to be associated 
                   with the temporary vector area that may 
                   be needed to perform the optimized vector 
                   instructions.  The default value is 20 
                   pages of physical memory for the temporary 
                   vector space (virtual array.) 
LO                 Obtains an object listing                    OC 
                   of the generated program, 
                   using the M$LO DCB. 
LS                 Obtains a source listing                     OC 
                   of the original symbolic 
                   source program, using the 
                   M$LO DCB. 
LU                 Lists update input on the                    OC 
                   M$LO DCB. 
MDM[AP]            Requests a mini-map (data                    OC 
                   map) that consists of the 
                   first level of a structure 
                   only (see DM) which currently 
                   has the same effect as the DM 
                   option. 
MPM[AP]            Requests a procedure map as                  OC 
                   in the PM option, but with 
                   the statement locations 
                   omitted from the map. 
MSC[HEMA]          Causes schema records to be                  PGC 
                   written only for referenced 
                   external or SYMDEFed items. 
MXR[EF] [ ( [ NLBL | NID ] ) ]                                  OC 
                   Produces a mini cross-reference 
                   summary of labels and/or identifiers. 
                   The NLBL sub-option suppresses label 
                   cross-referencing.  The NID sub-option 
                   suppresses identifier cross-referencing. 
                   (Currently the MXREF option has the 
                   same effect as the XREF option). 
NAM                Makes sure that argument checking            PGC 
                   is not performed by the linker when 
                   matching calling and receiving 
                   routines. 
NCF                Specifies no cracked format (see CF).        PGC 
                   The textual form of the FORMAT 
                   statement is passed to the run-time 
                   routines, which then have to decode 
                   the textual format and perform syntax 
                   checking. 
NDM                Suppresses the summary                       OC 
                   listing (DM). 
NDPS9[0]           Causes the vector intrinsics not to be       PGC 
                   recognized by the compiler.  If 
                   NDPS90 is requested, the compiler 
                   treats vector generic/intrinsic 
                   functions/calls as references to 
                   user-supplied functions or user- 
                   supplied subprograms; this is done 
                   to guard the integrity of pre-D00 
                   FORTRAN source programs.  NDPS90 
                   is the default for compilations 
                   performed on non-DPS 90 machines. 
                   (For run-time support routines 
                   vector generic/intrinsic 
                   functions/calls, see the DPS8 
                   compiler option.) 
NKAP               To disable, or turn off, the KAP             PGC 
                   vector optimizations.  This option 
                   is meaningful only if the target 
                   machine has vector instructions 
                   (such as the DPS90). 
NLO                Suppresses the object                        OC 
                   listing (LO). 
NLS                Suppresses LS; only source                   OC 
                   lines with errors are 
                   written. 
NLU                Suppresses listing of updates on             OC 
                   the M$LO DCB. 
NMP                Builds the object unit                       PGC 
                   without memory protection 
                   for the FORMAT statements. 
NOP                Suppresses code optimizing.                  PGC 
                   The optimization phase of 
                   the compilation process is 
                   skipped. 
NOU                Suppresses OU.                               OC 
NPM[AP]            Suppresses the procedure                     OC 
                   map listing (PM[AP].) 
NSC[HEMA]          Suppresses SC[HEMA].                         PGC 
NSO                Suppresses SO.                               OC 
NUI                Suppresses UI.                               IC 
NVOP               Suppress collapsing of DO                    PGC 
                   loops into vector instructions. 
                   This option is meaningful if 
                   the target machine is a DPS 90. 
NWA[RN]            Suppresses WA[RN].                           OC 
                   Miscellaneous information 
                   and warning message are not 
                   printed. 
NXR[EF]            Suppresses the cross-reference               OC 
                   listing (XREF). 
OP                 Optimizes code.                              PGC 
                   OP - optimizations performed 
                   include constant folding, common 
                   subexpression recognition, movement 
                   of subscript evaluations to the head 
                   of DO loops, and compile-time evaluation 
                   of intrinsic functions with constant 
                   arguments. 
                   OP(1) - all the OP optimizations plus 
                   code motion loop optimizations for all 
                   invariant expressions and strength 
                   reductions for DO loop control ids used 
                   in subsequent expressions. 
                   OP(2) - all the OP and OP(1) optimizations 
                   plus strength reductions for DO control 
                   id usages. 
OTD                Directs the compiler to                      PGC 
                   insure that the minimum number 
                   of DO loop interactions is one. 
                   Normally, a check is made 
                   for a zero trip DO loop prior 
                   to entering the DO loop. 
OU                 Produces an object unit and                  OC 
                   places it in a file, using 
                   the M$OU DCB, from which it 
                   can then be loaded and 
                   executed. 
PM[AP]             Requests a procedure map                     OC 
                   indicating the relative 
                   location of external entry 
                   points, local subroutines, 
                   and labels written through 
                   the M$LO DCB. 
R[OU]ND            Generate floating point instructions         PGC 
                   that yield rounded rather than floating 
                   point results. 
SC[HEMA]           Builds DELTA debug records                   PGC 
                   into the object unit 
                   generated for the program. 
SI                 Indicates to the compiler                    IC 
                   that input will be in 
                   symbolic (source) form, 
                   under control of the M$SI 
                   DCB. 
SO[=filename]      Produces a compressed output                 OC 
                   source program file from the 
                   original updated input using 
                   the M$SO DCB.  The new SO file key 
                   list numbers will be generated on 
                   the LS listing, if one is produced. 
SR[CH] (.ac1,      Gives the FORTRAN processor                  IC 
 .ac2...,.acn)     access to up to 8 (n<=8) 
                   accounts, in addition to the 
                   user's account and the :LIBRARY 
                   account, when searching for 
                   a named file in an INCLUDE 
                   statement.  A period (.) 
                   precedes each account number 
                   (ac).  The compiler first searches 
                   the n accounts in the order 
                   specified.  It then searches the 
                   user's account, then the :LIBRARY 
                   account.  Specifying passwords 
                   with the account and private disk 
                   pack accounts is also allowed. 
SV                 Saves the contents of local                  PGC 
                   variables from one invocation 
                   of a subprogram to the next 
                   invocation of the same 
                   subprogram.  Causes all local 
                   and dummy variables to be 
                   allocated in static storage. 
SYN                Terminates compilation after                 OC 
                   syntax checking the source 
                   program. 
UI                 Specifies that update                        IC 
                   records to be applied 
                   against the SI input are to 
                   be read from the M$UI 
                   device. 
UNIT(ALL)          Causes references to DCBs to be              PGC 
                   generated for all unit numbers 
  or               (constant values) that are 
                   associated with I/O calls (default). 
UNIT(NONE)         Suppresses DCB references for                PGC 
                   generating object units for 
  or               use in libraries. 
UNIT(n1            Specifies a DCB name to be                   PGC 
[,n2,...,ni])      generated by the program, where 
                   n is an integer value representing 
  or               a unit number. 
UNIT(ONLY,n1       States that only the                         PGC 
[,n2,...,ni])      specified list of unit numbers are 
                   to have DCB references generated 
                   for them in the following 
                   compilation. 
US                 Use as standard numbers:                     OC 
                   5 for input, 6 for list 
                   output, 7 for punch. 
                   (Normal defaults are 105 for 
                   input, 108 for list output, 
                   106 for punch.) 
UV                 Creates a summary listing of all             OC 
                   undeclared and undefined (but used) 
                   ID's in the user program. 
VCA                The virtual common allocation option         PGC 
                   causes  all the common entities of a 
                   common block that has been declared a 
                   virtual array to have unique V:blocks 
                   allocated for each.  This is for the 
                   case where the common block is declared 
                   differently in each subprogram.  One 
                   must still be aware that the common 
                   block virtual should be the first or 
                   only virtual "array" that is allocated 
                   in an extended data segment. 
VOP                Examines DO loops and, if                    PGC 
                   appropriate, collapses them 
                   into vector instructions. 
                   For the VOP option to take 
                   effect, the DPS90 compilation 
                   option must also be selected, or 
                   the compilation must be performed 
                   on a DPS 90 machine. 
WA[RN]             Requests that all warning                    OC 
                   messages generated by the 
                   compiler be written through 
                   the M$DO DCB. 
XR[EF] [ ( [ NLBL | NID ] ) ]                                   OC 
                   Produces a cross-reference summary 
                   of labels and/or identifiers. 
                   The NLBL sub-option suppresses label 
                   cross-referencing.  The NID sub-option 
                   suppresses identifier cross-referencing. 
X, XD, Y, or Z     Treats as valid, and                         IC 
                   therefore compiles FORTRAN 
                   statements with X, D, Y, or 
                   Z in column 1.  If these 
                   options are not selected, 
                   input records with X, D, Y, or 
                   Z in column 1 will be 
                   treated as comment lines. 
                   When the X, XD, Y, or Z option 
                   is selected, lines with the 
                   corresponding characters in 
                   column 1 are compiled, 
                   leaving the user the choice 
                   of whether or not to 
                   compile a given line in 
                   consecutive compilation 
                   runs by changing his 
                   options.  This is called 
                   conditional compilation. 
Conflicting compiler options are listed in the following table. 
  Option             Conflicting Options 
DIAG or            Alternate output forms:  OU, LO, NSC, 
SYN                              PM, DM, MDM 
IMR                Alternate memory initialization directive:  IMZ 
FF                 ANS 
NOU                LO, PM, MDM, MSC, DM, SC, MPM 
00001
00002
00003
00004
00005
00006
00063
00007
00008
00009
00010
00011
00012
00013
00014
00015
00016
00017
00018
00019
00020
00021
00022
00023
00024
00025
00026
00027
00028
00028
00029
00030
00031
00032
00033
00034
00035
00036
00037
00038
00039
00040
00041
00042
00043
00044
00045
00046
00047
00047
00048
00048
00049
00050
00051
00052
00053
00054
00055
00056
00057
00058
00059
00060
00061
00062
The compilation options can be divided into input controls (IC), output 
controls (OC), and program-generation controls (PGC).  Input controls are 
concerned with source program location and interpretation, and column-1 
control.  Output and program-generation controls are self-explanatory. 
The following table lists the FORTRAN-77 compilation options by group. 
  Group                 Option         Description 
Input Control         BC             Batch compilation 
Options (IC)          FF             Free form 
                      SI             Symbolic (source) input 
                      SR[CH]         Search list 
                      [N]UI          Update input 
                      X, Y, Z, XD    Conditional compilation 
Output Control        DIAG           Diagnostic compilation only 
Options (OC)          [N]DM          Data Map 
                      DO             Diagnostic output 
                      [N]LO          List output 
                      [N]LS          List source 
                      [N]LU          List update source 
                      MDM[AP]        Partial data map 
                      MPM[AP]        Partial procedure map 
                      [N]OU          Object unit file output 
                      [N]PM[AP]      Request procedure map 
                      [N]SO          Updates source input 
                      SYN            Terminate compilation after syntax 
                                     check 
                      US             Use alternate default DCBs 
                      UV             List undefined and undeclared 
                                     program variables 
                      [N]WA[RN]      Request warning messages 
                      [N|M]XR[EF]    Request cross referencing 
Program Generation    ADP            Automatic double precision 
Control (PGC)         ANS            ANS features only 
                      CC             Common in constant storage 
                      CDS            COMMON in data segments 
                      CF             Cracked format 
                      DBM            Data Base Management 
                      DOZ            Allows IBM form of hexadecimal 
                                     constants 
                      DPS8           Vector intrinsics for non-DPS90 
                                     machines 
                      DPS9[0]        Vector intrinsics for DPS90 machine 
                      IMR            Initialize memory randomly 
                      IMZ            Initialize memory to zero 
                      [N]KAP         KAP vector optimizations 
                      MSCH[EMA]      Schema records for referenced 
                                     external, EXT, or SYMDEFed items 
                      NAM            No argument matching 
                      NCF            No cracked format 
                      NMP            No memory protection 
                      NDPS9[0]       No vector intrinsics 
                      [N]OP          Optimize code 
                      [N]SC[HEMA]    Debug schema 
                      [N]VOP         Optimize vector DO loops. 
                      OTD            One Trip DO loops 
                      R[OU]ND        Generate rounded results 
                      SV             Save local variables 
                      UNIT           DCB Specification 
                      VCA            Virtual common allocation 
NOTE:  [N] indicates suppress; e.g., NDM indicates suppress data map. 
In FORTRAN-77, several statements may be written on the same line if they are 
separated by semicolons.  For example, 
    A=4.5;B=Q**2;WRITE (2) A,B;GO TO 8 
is equivalent to the statements 
    A=4.5 
    B=Q**2 
    WRITE (2) A,B 
    GO TO 8 
Such statements are called compound statements and can be used to reduce the 
number of lines in the source program.  This feature also allows the 
programmer to express many statements in a normal manner, as in 
    X=SIN(A);Y=COS(A);Z=CMPLX(Y,X) 
When a compound statement is labeled, the label defines the location of the 
first part of the compound statement.  Therefore, 
    DO 45 I=N,M 
 45 A(I)=Q**I;B(I)=Z**I 
is equivalent to 
    DO 45 I=N,M 
 45 A(I)=Q**I 
    B(I)=Z**I 
When a compound statement appears as the statement in a logical IF statement, 
the entire compound statement is executed when the logical expression being 
tested is "true".  Consequently, the statements 
    IF (A .GT. B), E=I*R;Z=CZ+R+LZ 
 51 W=T+B/X 
are equivalent to 
    IF (A .LE. B), GO TO 51 
    E=I*R 
    Z=CZ+R+LZ 
 51 W=T+B/X 
Compound statements are permitted only with executable statements. 
Nonexecutable statements are not allowed. 
GO_TO_COMPUTED
COMPILER_OPTION_GROUPS
COMPILER_OPTIONS
The following are FORTRAN-77 major concepts: 
o   Executable Program - a program consisting of a FORTRAN main program, any 
    number of FORTRAN subprograms, and any number of associated subprograms 
    defined by a means other than FORTRAN, such as assembly language, etc. 
o   Program Unit - a set of FORTRAN statements and optional comment lines, 
    with the last FORTRAN statement an END statement.  A program unit can be 
    either a FORTRAN main program or a FORTRAN subprogram. 
o   FORTRAN Main Program - an executable program that may or may not be headed 
    by a PROGRAM statement.  The first statement of a FORTRAN main program 
    must not be a FUNCTION, SUBROUTINE, or BLOCK DATA statement. 
o   FORTRAN Subprogram - a program headed by either a FUNCTION SUBROUTINE or 
    BLOCK DATA statement.  A function subprogram is headed by a FUNCTION 
    statement and is executable, as is a subroutine subprogram which is headed 
    by a SUBROUTINE statement.  A block data subprogram, headed by a BLOCK 
    DATA statement, is nonexecutable. 
o   Program Content - data and FORTRAN procedure if the program is executable; 
    data only if the program is nonexecutable. 
o   FORTRAN Procedure - a procedure specified by executable FORTRAN 
    statements.  Some specifications such as SUBROUTINE and FUNCTION also 
    result in FORTRAN procedure. 
o   Data - the data in a program unit, as represented by constants, variables, 
    arrays, and array elements.  A constant is a data item whose value is 
    explicitly stated, for example, the integer 5 represented as "5", or the 
    number PI represented to three decimal places as "3.142."  A constant may 
    also be represented in a program unit by a symbolic name (see 
    PARAMETER_STATEMENT).  A variable is a symbolic name whose value may vary 
    during program execution.  An array is an ordered set of data whose values 
    may vary during program execution, and whose symbolic name is declared in 
    an array declarator (see ARRAY_DECLARATOR).  An array element is 
    represented by an array name qualified by a subscript list enclosed in 
    parentheses. 
In FORTRAN-77, source lines of a FORTRAN statement may have the characters X, 
Y, or Z in column 1.  Such statements are known as conditional compilation 
statements, which are processed only when the corresponding X, Y, or Z is 
selected as an option on the FORTRAN invocation line.  Otherwise, the line is 
treated as a comment line. 
The conditional compilation feature enables the programmer to include in a 
program additional statements for checkout purposes, such as intermediate 
output and special error checking.  When checkout has been completed, these 
statements do not have to be removed from the program.  Instead, the program 
is compiled without the corresponding X, Y, or Z option, with the result that 
the conditional compile statements are treated as comments only.  The 
statements remain in the listing, however, and may serve as documentation of 
checkout procedures.  Also, they can easily be reinstated at any time. 
If a conditional compile statement requires continuation lines, these must 
also have the appropriate X, Y, or Z in column 1.  A normal (not conditional) 
source line must not be continued utilizing a conditional compilation line. 
The following figure shows examples of conditional compilation and 
continuation source lines. 
          !FORTRAN(X,Y) 
          FORTRAN 77 VERSION E02  AUG 14 '92 
              10>X     PRINT 3,A, B, C, D, E, F 
              20>    3 FORMAT(X,6F.7) 
              30>Y     PRINT 3,U,V,W,X,Y,Z 
              40>      END 
            X-LINES COMPILED: 1 
            Y-LINES COMPILED: 1 
          ERRORS FOUND       : 0      TOTAL ERRORS FOUND: 0 
CONSTANT_STATEMENT
Constants are data that do not vary in value and are referenced either by 
value or by a symbolic name specified in a PARAMETER statement.  A constant 
name is a self-identifying character string which represents the type and 
value of a constant.  There are constants for each type of data.  An unsigned 
constant is a constant without a leading sign.  A signed constant is a 
constant with a leading plus or minus sign.  An optionally signed constant is 
a constant that may be either signed or unsigned.  Integer, real, and double 
precision constants may be optionally signed constants, except where specified 
otherwise.  The following is a list of constants: 
INTEGER 
REAL 
DOUBLE_PRECISION 
COMPLEX 
DOUBLE_PRECISION_COMPLEX 
LOGICAL 
CHARACTER 
LITERAL 
HOLLERITH 
HEXADECIMAL 
OCTAL 
To display a particular constant (such as Logical), enter: 
HELP (FORTRAN) CONSTANTS LOGICAL 
To receive a display of all constants, enter ??. 
INTEGER 
An integer constant is written as a string of digits whose magnitude must not 
exceed 34,359,738,367. 
Examples: 
    382     997263     1000000000     000546     8 
     13       1961      323344224     372436     0 
REAL 
A real constant differs from an integer constant by having a decimal point or 
an exponent or both.  The exponent follows the numeric value and consists of 
the letter E followed by a signed or unsigned integer that represents the 
power of ten by which the numeric value is to be multiplied.  The following 
forms are permissible: 
    n.m        n.        .m 
    n.mE[+]e   n.E[+]e   .mE[+]e   nE[+]e 
where 
n, m, and e    are each a string of digits, and the plus sign preceding e is 
optional. 
For example, .567E5 has the meaning .567 x 10**5 and can also be represented 
by any of the following equivalent forms: 
    0.567E+05     5.67E4     56700. 
    567000E-1     567E02     56700.000E-00 
The value of a real constant may not exceed the value limits for real data. 
Any number of digits may be written in a real constant, but only the seven 
most significant digits are retained. 
Since any real constant may be written in a variety of ways, the user can 
choose the form he prefers. 
Examples: 
    5.0      7.6E+5       3.14159265358979323846 
    0.01     6.62E-27     5878550402984.00 
DOUBLE_PRECISION 
A double precision constant is written like a real constant, except that the 
letter D is used in the exponent instead of the letter E.  The exponent must 
be present to denote a constant specifically as double precision.  Thus, a 
double precision constant may be written in any of the following forms: 
    n.mD[+]e     n.D[+]e     .mD[+]e     nD[+]e 
where 
n, m, and e    are each of a string of digits.  The plus sign preceding the e 
is optional, and D signifies a double precision constant. 
The value of a double precision constant may not exceed the value limits for 
double precision data.  Any number of digits may be written in a double 
precision constant, but only the 18 most significant digits are retained. 
COMPLEX 
A complex constant is written as an ordered pair of real or integer constants 
in the form: 
    (c1,c2) 
where 
c1 and c2    may be either signed or unsigned.  If either c1 or c2 is an 
integer value, it is converted to a real data value. 
The complex constant (c1,c2) is interpreted as meaning c1 + c2i, where 
i=(-1)**(.5).  Thus the following complex constants have values as indicated: 
    (1.34,52.01)         =    1.34 +  52.01i 
    (98344.,0.34452E+02) = 98344.0 +  34.452i 
    (-1.,-1000.)         =    -1.0 - 1000.0i 
Neither part of a complex constant may exceed the value limits established for 
real data. 
DOUBLE_PRECISION_COMPLEX 
A double precision complex constant (described as type DOUBLE_COMPLEX) is 
written like a complex constant.  If either the real or imaginary part is a 
double precision constant, the complex constant becomes a double precision 
complex constant.  If one part is real or integer, it is converted to double 
precision. 
Examples: 
    (.757D6,3D-4)  = 757000.0D0   +   .0003D0i 
    (7.0D0,0.0)    =      7.0D0   +  0.0D0i 
    (-4.286D0,1.3) =     -4.286D0 +  1.3D0i 
Neither part of a double precision complex constant may exceed the value 
limits established for double precision data. 
LOGICAL 
The logical constants "true" and "false" are written as .TRUE.  and .FALSE., 
respectively.  In DATA statements the letters T for .TRUE.  and F for .FALSE. 
may be used. 
CHARACTER 
A character constant is written as: 
's' or "s" 
where 
s    is a string of characters, and 
' or "    are delimiters. 
The delimiting apostrophes or quotation marks are not part of the data 
represented by the constant.  Note that blanks are significant in character 
constants. 
Character constants may appear in the following contexts: 
1.  As an argument to a function or subroutine corresponding to a dummy 
    argument of type character. 
2.  As a constant item in a DATA statement corresponding to an identifier of 
    type character. 
3.  As a list item in an OUTPUT or a WRITE statement 
4.  As the format specification in either a READ or WRITE statement 
5.  In a PAUSE or STOP statement 
6.  In a character expression 
When the delimiter is an apostrophe, two consecutive apostrophe marks may be 
used to represent a single apostrophe (') within the character constant.  For 
example, 'AB''CD' is interpreted as the character value AB'CD.  When the 
delimiter is a quotation mark, two consecutive quotation marks may be used to 
represent a single quotation mark.  For example, "AB""CD" is interpreted as 
the character value AB"CD.  Apostrophe (') marks or quotation (") marks 
separated by blanks are not considered to be consecutive. 
Examples: 
    'AB"CD'                  interpreted as AB"CD 
    "'ALPHANUMERIC'"         interpreted as 'ALPHANUMERIC' 
    '''ALPHANUMERIC'''       interpreted as 'ALPHANUMERIC' 
    "'DON'T!'"               interpreted as 'DON'T!' 
    '(X+Y)='                 interpreted as (X+Y)= 
    "(X+Y)="                 interpreted as (X+Y)= 
    'A""B'                   interpreted as A""B 
Usage Notes: 
The apostrophe (') and quotation (") marks cannot be used interchangeably 
within a given character constant, i.e., the same delimiter must be used at 
the beginning and end of a character string.  Any occurrence of the delimiter 
within the string is interpreted as the end of the string, unless it is 
immediately followed by the same delimiter. 
Null character strings are recognized by FORTRAN-77.  Two successive 
apostrophes ('') followed by a blank denote a null character string.  The null 
character will be used for a null character string.  For ASCII codes, enter 
HELP(IBEX)ASCII. 
LITERAL 
A literal constant is written as: 
nHs 
where 
s    is a string of characters.  Note that blanks are significant in literal 
constants. 
n    is an unsigned integer specifying the number of characters in the string. 
Literal constants may appear in the following contexts: 
1.  As an argument to a function or subroutine 
2.  As a constant item in a DATA statement 
Examples: 
    2H7: 
    8H'DON'T!' 
    6H(X+Y)= 
Note that when n<8, the construct can be used as either a literal or Hollerith 
constant.  The use of Hollerith constants, octal constants and hexadecimal 
constants is an extension of the ANS standard language.  These features are 
included for compatibility with other systems.  The use of these forms of 
constants is machine-dependent and not recommended.  The appearance of these 
constants is restricted. 
HOLLERITH 
A Hollerith constant is written either as: 
nHs  or  nRs 
where 
n    is an unsigned integer (0<n<8) specifying the number of characters in the 
string. 
s    is a string of exactly n characters.  Each character in a Hollerith 
string may be one of the set of acceptable FORTRAN-77 characters.  Note that 
blanks are significant in a Hollerith string and must therefore be counted in 
determining the value of n. 
H    indicates that the characters are to be left-justified in storage, with 
the remaining 8-n characters filled with the blank character.  The data type 
of a Hollerith constant is context dependent.  As an assignment or a 
relational operand, the type is the type of the other associated operand.  If 
used as a subscript, the type is integer. 
R    indicates that the characters are to be right-justified in storage, with 
the leading 8-n characters set to binary zeros.  The R form of a Hollerith 
constant is a nonstandard special form of integer constant and is not 
meaningful in any other context (see LITERAL under the topic CONSTANTS). 
Examples: 
6HPENCIL    2H$$      1R+       2H X        3R-.7      4HCLUB  1H      3R'S' 
Constant          Storage Format            Constant         Storage Format 
--------          --------------            --------         -------------- 
0H          blank  blank  blank  blank      0R          zero  zero  zero  zero 
1HA           A    blank  blank  blank      1R1         zero  zero  zero    1 
2HX=          X      =    blank  blank      2RBI        zero  zero    B     I 
3HERR         E      R      R    blank      3RTRI       zero    T     R     I 
4H4396        4      3      9      6        4RFOUR        F     O     U     R 
The character zero is not the same as binary zero; thus, for example, 4H0003 
is not the same as 1R3 (and neither of them is equal to an integer 3).  For 
manipulation of characters as true right-justified integers, the R form must 
be used. 
In executable statements, Hollerith constants are allowed only in simple 
assignment statements, such as A=4H1234, or operands in relational comparisons 
such as A.EQ.4H1234. 
HEXADECIMAL 
Hexadecimal constants are also a special form of integer constant in most 
contexts.  They are written as 
nZs 
where 
n    is an unsigned integer specifying the number of digits in the string. 
s    is a string of hexadecimal digits.  These digits are 0, 1, 2, 3, 4, 5, 6, 
7, 8, 9, A, B, C, D, E, and F. 
Normally, n must lie in the range 1 < n < 9.  When used in DATA initialization 
statements, however, up to 18 digits may be specified.  (See DATA_STATEMENT.) 
In forming word integers, hexadecimal constants are right-justified and 
zero-padded to the left. 
In DATA statements, hexadecimal constants are used exactly as written.  They 
are not assumed to be integer, and no type conversion is performed. 
Examples: 
    9Z010400000     4ZF4E9     6Z1D2C7E     9ZFFFF20000 
Hexadecimal constants are allowed in simple assignment statements such as 
I=4Z1234.  They are also allowed as operands in relational expressions. 
OCTAL 
Octal constants are also a special form of integer in most contexts.  They are 
written as 
nOs 
where 
n    is an unsigned number specifying the number of digits in the string. 
s    is a string of octal digits.  These digits are 0, 1, 2, 3, 4, 5, 6, and 
7. 
Normally, n must be in the range 1<n<12.  When used in DATA statements, 
however, up to 24 digits may be specified.  (See DATA_STATEMENT.) 
In forming word integers, octal constants are right-justified and zero-padded 
to the left. 
In DATA Statements, octal constants are used exactly as written.  They are not 
assumed to be integer, and no type conversion is performed. 
Octal constants are allowed in simple assignment statements, such as: 
I=30123.  They are also allowed as operands in relational expressions. 
Examples: 
    12O000567743210     4O1234     5O76543 
00007
00004
00003
00005
00011
00010
00001
00009
00006
00012
00002
00008
Syntax: 
CONSTANT [,]p1=e1,p2=e2,...,pn=en 
Parameters: 
n    is greater than zero. 
pi    is a symbolic name. 
e1    is a constant (excepting literal, Hollerith, octal, or hexadecimal 
constants) or a constant expression. 
Description: 
The CONSTANT statement is used to define the symbolic name of a constant.  A 
constant expression consists of numeric constants or previously declared 
parameters operated upon by the arithmetic operators +, -, *, / and **. 
Constant expressions are evaluated by the compiler and yield a constant 
result.  Operands may be of differing type with conversion performed as in 
arithmetic expressions. 
The symbolic name of a constant must be defined prior to its usage and may be 
used wherever a constant may be used, except that it may not appear in a 
FORMAT.  Thus, the term constant in the above discussion refers to either a 
constant or the symbolic name of a constant. 
The symbolic name assumes (inherits) the type of the constant; any other 
explicit type declaration for the name is diagnosed as redundant or 
conflicting. 
Although a comma after CONSTANT in the statement form above is optional, its 
inclusion is recommended to preclude misinterpretation of the statement if it 
is used incorrectly. 
The CONSTANT statement is identical in form to the CP-V PARAMETER statement. 
(See PARAMETER_CPV.) 
Examples: 
Examples of definition: 
    CONSTANT M=1,K=2,IOU=4 
    CONSTANT W=10 
    CONSTANT TEXT='THIS IS A CONSTANT' 
    CONSTANT, OTHER='ABLE BAKER CHUCK' 
Examples of usage for the above definitions: 
    IMPLICIT CHARACTER*(K)(O-R) 
    REAL Z1(W,W) 
    CHARACTER*(IOU),B,C*21 
    DO 10,I=1,W 
 10 Z1(I,IOU)=0.0 
00002
00003
00001
For programs originating in an external source file (i.e., not entered 
interactively), if another line is needed to complete the statement, a 
continuation mark - any character other than blank, zero, or tab - is placed 
in column 6 of the continuation line, and the statement continuation is placed 
in the statement field (columns 7 through 72) of the continuation line.  The 
label field of a continuation line is ignored. 
For programs entered interactively at a terminal, column 6 is not used as a 
continuation field.  Instead, the user ends a line with a colon (:) to 
indicate that the line is to be continued, and the compiler prompts with '<>' 
at the beginning of the next line to indicate that this will be a continuation 
line. 
Syntax: 
CONTINUE 
Description: 
The CONTINUE statement does not cause the compiler to generate machine 
instructions, and consequently has no effect on a running program.  The 
purpose of the statement is to allow the insertion of a label at any point in 
a program.  CONTINUE statements are most often used as the terminal statement 
of a DO or REPEAT range, as in the following example. 
Examples: 
       REPEAT 72, WHILE Q .AND. P 
       . 
       . 
       . 
       IF (X**I+.9999E-5)72,72,88 
    72 CONTINUE 
    88 H(33)=T(3,R,L,E)/22.5 
       . 
       . 
       . 
00001
00002
CONTINUE
FORTRAN-77 Control Statements are covered under the following HELP topics: 
GO_TO_UNCONDITIONAL 
GO_TO_ASSIGNED 
ASSIGN 
ARITHMETIC_IF 
LOGICAL_IF 
BLOCK_IF 
ELSE_IF 
END_IF 
CALL 
RETURN_FORM1 
RETURN_FORM2 
DO 
REPEAT_WHILE 
REPEAT_FOR 
CONTINUE 
PAUSE 
STOP 
END 
See also:  LABELS. 
The CP-V PARAMETER statement and the CP-6 CONSTANT statement are identical. 
The CP-V PARAMETER statement is used to define the symbolic name of a constant 
and is of the form: 
PARAMETER [,]p1=e1,p2=e2,...pn=en 
where 
n    is greater than zero. 
pi    is a symbolic name. 
ei    is a constant (expecting literal, Hollerith, or hexadecimal constant) or 
a constant expression. 
A constant expression consists of numeric constants or previously declared 
parameters operated upon by the arithmetic operators +, -, *, / and **. 
Constant expressions are evaluated by the compiler and yield a constant 
result.  Operands may be of differing type with conversion performed as in 
arithmetic expressions. 
DATA_STATEMENT
Syntax: 
c1,c2,c3,...,cm 
Parameters: 
ci    are either constants or repeated constants of the following forms: 
    c 
    r*c 
    where 
    c    is a signed or unsigned constant of an appropriate type of any of the 
    forms described under CONSTANTS, including literal constants. 
    r    is an unsigned integer repeat count, whose value (nonzero) indicates 
    the number of times the constant is to be repeated. 
Allowed and not-allowed data type correspondences in a DATA statement are 
tabulated below. 
----------+----------------------------------------------------------- 
          |                   VARIABLE TYPE 
----------+----------------------------------------------------------- 
          |Integer*2                                        Logical*1 
CONSTANT  |and          Double            Double            and 
          |Integer Real Precision Complex Complex Character Logical 
----------+------- ---- --------- ------- ------- --------- --------- 
Integer   |   X     F       F        F       F        N        N 
          | 
Real      |   I     X       P        F       F        N        N 
          | 
Double    | 
Precision |   I     P       X        F       F        N        N 
          | 
Complex   |   I     F       F        X       P        N        N 
          | 
Double    | 
Complex   |   I     F       F        P       X        N        N 
          | 
Character |   B     B       B        B       B        X        B 
          | 
Logical   |   N     N       N        N       N        N        X 
          | 
Hex~      |  L<9   L<9     L<18      N       N        N       L<9 
          | 
Octal~    |  L<12  L<12    L<24      N       N        N       L<12 
          | 
Character | 
(Literal)~|   A     A       A        A       A        X        A 
          | 
Hollerith |   E     E       E        E       E        E        E 
          | 
--------------------------------------------------------------------- 
~Special constant forms available for compatibility but not 
 recommended because of machine dependency and extension to the 
 standard. 
--------------------------------------------------------------------- 
The symbols used in the above table have the following meanings: 
Symbol     Meaning 
------     ------- 
A, E    Represent allowed initializations using 'long' constants.  (A long 
        constant contains more characters than the data type allows.)  This is 
        the only case where a constant may be used to initialize more than one 
        element of an array whose identifier appears in the variable list. 
B       Means correspondence is allowed (see below). 
F       The variable is assigned the real or double precision approximation of 
        the value.  Note that since real precision is less than that of 
        integers, conversion to real may cause a loss of significant digits. 
I       The value is truncated to integer.  The truncated value is equal to 
        the sign of the expression times the greatest integer less than or 
        equal to the absolute value of the expression (e.g., 4274.9983 is 
        truncated to 4274, and -0.6 to 0).  Values that are greater than the 
        maximum size of an integer will be truncated at the high-order end as 
        well.  Results in this case generally are not meaningful. 
L       The number of hexadecimal digits in the constant; the maximum number 
        permitted is specified. 
N       Not allowed. 
P       The precision of the value is increased or decreased accordingly. 
X       Direct assignment of the exact value. 
Usage Notes: 
A character constant or a Hollerith constant used to initialize a variable 
must not contain more characters than the variable may hold, that is: 
    Data Type            Maximum Number 
    of Variable          of Characters 
    -----------          -------------- 
    Integer*2                  2 
    Integer                    4 
    Real                       4 
    Double precision           8 
    Complex                    8 
    Double complex            16 
    Logical                    4 
    Logical*1                  1 
The exception to the maximum number of characters is that the constant may 
contain up to 511 characters when the item in the variable list is an array 
name.  In this case each array element is initialized with a portion of the 
constant until the constant is depleted. 
Examples: 
The following example illustrates some of the features described above: 
    CHARACTER*4 MM(10) 
    COMPLEX C1,C2 
    DATA (MM(J),J=1,4),C1,C2/'ABCD','EF','GH','17',2*(-4.,17.8)/ 
The preceding DATA statement causes the same assignments as the following set 
of statements: 
    MM(1)='ABCD' 
    MM(2)='EF' 
    MM(3)='GH' 
    MM(4)='17' 
    C1=(-4.0,17.8) 
    C2=(-4.0,17.8) 
When an array name appears without subscripts in the variable list, it 
references the entire array in storage order, as in an input/output list. 
(See SIMPLE_LISTS under IO_LISTS, i.e., IO_LISTS SIMPLE_LISTS.)  Thus, it 
requires as many constants as there are elements in the array. 
There must be as many items in the variable list as in the constant list; 
otherwise, some of the constants will not be used.  If there are more items in 
the variable list than in the constant list, the statement is in error. 
As an example, if E is an array, declared as 
    REAL E(11, 11) 
the following DATA statements are equivalent: 
    DATA ((E(J,K),J=1,11),K=1,11)/121*1.0/ 
    DATA E/121*1.0/ 
When using nested implied DO loops to initialize arrays in DATA statements, 
the order in which the loops are nested affects the compilation and linking 
efficiency.  Nested loops which initialize an array's elements in the order 
the elements are allocated in memory use less memory and time to compile and 
link.  (See ARRAY_STORAGE for a description of how arrays are allocated in 
memory.)  For example, the above DATA statements are more efficient than 
    DATA ((E(J,K), K=1,11), J=1,11)/121*1.0/ 
although both statements will initialize E to all zeroes. 
Dummy variables may not be initialized with the DATA statement, while 
variables that are local, GLOBAL, or in COMMON may be. 
If a variable is initialized more than once in a program, the results are 
unpredictable.  Initializing the same data location with two different values 
is not detected as an error, but is strongly discouraged as bad programming 
practice.  If a GLOBAL or labeled COMMON variable is initialized in more than 
one program, its value will depend on which program is loaded first.  This 
practice is not recommended. 
A DATA statement may not appear in a compound statement. 
Note that, in Bull FORTRAN-77, labeled COMMON variables may be initialized in 
any program. For compatibility with other FORTRAN systems, however, the BLOCK 
DATA subprogram is provided and may be used for this purpose. 
00003
00001
00002
ENCODE_DECODE
Format specifications describe the kind or type of conversion to be performed, 
specific data to be generated, print positioning, scaling of data values, and 
editing to be executed.  Each integer, real, double precision, character, or 
logical datum appearing in an input/output list is processed by a single data 
conversion format specification, while complex data are operated on by two 
consecutive format specifications.  Format specifications may have any of the 
forms shown in the following table: 
                       FORTRAN-77 Format Specifications 
                                                Extension to 
       ANS Standard                             ANS Standard 
       ------------                             ------------ 
      rFw.d      rLw    TLi                          rZw 
      rEw.d      rAw    r/                           rOw 
      rEw.dEe    r's'   S                            rMw 
      rDw.d      nHs    : (colon)                    r"s" 
      rGw.d      iP     BZ                           r$s$ 
      rGw.dEe    iX     SS                           rRw 
      rIw        TW     BN                           rEw.dDe 
      rIw.m      TRi    SP                           rGw.dDe 
where 
the characters F,E,D,G,I,S,L,A,R,O,Z,M,H,$, quotemark ("), apostrophe ('), 
SS,SP,BZ,BN,X,T,P, colon (:), and slash (/) define type of conversion, data 
generation, scaling, editing, and FORMAT control (print positioning). 
Parameters: 
r    is an optional, unsigned integer that indicates that the specification is 
to be repeated r times.  When r is omitted, its value is assumed to be 1.  For 
example, 3I6 is equivalent to I6,I6,I6.  However, 3'ABCD' is not equivalent to 
'ABCD', 'ABCD', 'ABCD' on an output operation. 
w    is an optional unsigned integer that defines width in characters 
(including digits, decimal points, algebraic signs, and blanks) of the 
external representation of the data being processed.  If w is not present in a 
specification, the size of the external field depends on the characteristics 
of the data and the type of conversion performed, as described individually 
for each specification. 
d    for F,E,D, and input G specifications, is an optional, unsigned integer 
that specifies the number of fractional digits appearing in the magnitude 
portion of the external field.  If d is not present, its value is assumed to 
be zero, and the decimal point character preceding it should not appear.  That 
is, Ew.0 and Ew are equivalent. 
For output G specifications, d is also an unsigned integer, but in this 
context it is used to define the number of significant digits that appear in 
the external field; therefore, its value should not be zero. 
e    is an unsigned integer that defines the number of exponent digits 
appearing in the exponent field. 
n    is an unsigned integer that defines the number of characters being 
processed. 
m    is an unsigned integer that defines the minimum number of digits to 
appear in the external field. 
s    is a string of characters acceptable to the FORTRAN-77 processor. 
i    is a signed integer (plus signs are optional).  The function of i is 
described under X,P,TR, and TL specifications. 
See ADJUSTABLE_FORMATS for further details on r, w, d, e, and i. 
To list related subtopics, enter: 
HELP (FORTRAN) FORMATS TOPICS 
To display a particular format (such as F format), enter: 
HELP (FORTRAN) FORMATS F_FORMAT 
To receive a display of all formats, enter ??. 
See Also: 
HELP (FORTRAN) PARENTHESIZED_FORMATS 
HELP (FORTRAN) ADJUSTABLE_FORMATS 
HELP (FORTRAN) PRINT_POSITIONING 
F_FORMAT 
The form is: 
rFw.d 
Integer, real, double precision, or either part of complex data may be 
processed by this form of conversion.  Double precision values are converted 
with full precision if sufficient width is specified by w, and the value of d 
allows for the appropriate number of digits in the fractional portion of the 
field. 
Output (F Format) 
OUTPUT_F_FORMAT 
Internal values are converted to real constants, rounded to d decimal places 
with an overall length of w.  The field is right-justified with as many 
leading blanks as necessary.  Negative values are preceded with a minus sign. 
Consequently, for the specification F11.4, 
       273.4     is converted to     273.4000 
           7     is converted to       7.0000 
       -.003     is converted to       -.0030 
  -442.30416     is converted to    -442.3042 
When no width is specified (i.e., w is not present), the converted field 
contains only the number of digits necessary to express the value, plus one 
blank to the right of the field.  Therefore, for the specification F.1, 
  349.5203       is converted to    349.5b 
     70000       is converted to    70000.0b 
       -22       is converted to    -22.0b 
and for the specification 2F.4, the output list 
  .03359,-67     is converted to    .0336b-67.0000b 
If a value requires more positions than are allowed by the magnitude of w, the 
entire field width is filled with asterisks.  This is not an error condition. 
Thus, for the specification F6.4, 
  -1.22315       is converted to    ****** 
  432034.        is converted to    ****** 
In order to ensure that such a loss of digits does not occur, the following 
relation must hold true: 
    W>d+2+n 
where n is the number of digits to the left of the decimal point.  Note that 
the 2 in d+2+n allows for the decimal point character and the sign position in 
the output field. 
Input (F Format) 
INPUT_F_FORMAT 
Input strings may take any of the integer, real, or double precision constant 
forms discussed under NUMERIC_INPUT_STRINGS.  Each string is of length w with 
d characters in the fractional portion of the value.  If a decimal point is 
present in the input string, the value of d is ignored, and the number of 
digits in the fractional portion of the value are explicitly defined by the 
decimal point.  For the specification F10.3, 
      33      is converted to         .033 
  802142      is converted to       802.142 
  .34562      is converted to        .34562 
  -7.001      is converted to        -7.001 
If the width w is not specified, conversion starts with the first nonblank 
character in the input string and ends with the first comma or blank that 
follows a digit or a decimal point.  The comma or blank is bypassed before 
conversion of the next field begins.  For the specification 2F.2, the string 
333,.003 
is converted to the values 
3.33 .003 
E_FORMAT 
There are two E Formats:  (1) E Format Normalized, with E Exponent; and (2) E 
Format with Exponent Specifier. 
The forms are: 
Normalized, with E Exponent:      rEw.d 
With Exponent Specifier:          rEw.dEe  or  rEw.dDe 
where 
e    specifies the number of digits in the exponent field, and 
must be in the range 0 < e < 9. 
Integer, real, double precision, or either part of complex data may be 
processed by this form of conversion.  Double precision values are converted 
with full precision if sufficient width is specified by w and the value of d 
allows for the appropriate number of digits in the fractional portion of the 
field. 
Output (E Format) 
OUTPUT_E_FORMAT 
Internal values are converted to constants of one of the forms shown below: 
 E Format Normalized,      E Format                   E Format 
 with E Exponent*          with E Exponent            with D Exponent 
 Converts Internal         Specifier Converts         Specifier Converts 
 Value to:                 Internal Value to:         Internal Value to: 
 --------------------      --------------------       -------------------- 
 .ddd...dE+ee              .ddd...dE+xx...x           .ddd...dD+xx...x 
 where ddd...d repre-      where ddd...d repre-       where ddd...d repre- 
 sents d digits,           sents d digits,            sents d digits, 
 and +ee or -ee is         and +xx or -xx is          and +xx or -xx is 
 is interpreted as a       is interpreted as a        is interpreted as a 
 multiplier of the         multiplier of the          multiplier of the 
 form:  10+ee              form:  10+xx...x           form:  10+xx...x 
 * If the exponent requires more than two digits, the E is omitted: 
   .ddd...d+eee 
Internal values are rounded to d digits, and negative values are preceded by a 
minus sign.  The external field is right-justified and preceded by the 
appropriate number of blanks.  The following table presents examples of the 
conversion of internal values. 
                  Normalized with   E Exponent          D Exponent 
                  E Exponent.       Specifier.          Specifier. 
                  Specification     Specification       Specification 
  Internal        E15.8 Converts    E16.8E3 Converts    E15.8D2 Converts 
  Value           Value to:         Value to:           Value to: 
  ----------      ---------------   -----------------   ---------------- 
     90.4450       .90445000E+02     .90445000E+002      .90445000D+02 
  -435739015.     -.43573902E+09    -.43573902E+009     -.43573902D+09 
     .000375       .37500000E-03     .37500000E-003      .37500000D-03 
          -1      -.10000000E+01    -.10000000E+001     -.10000000D+01 
          .2       .20000000E+00     .20000000E+000      .20000000D+00 
         0.0       .00000000E+00     .00000000E+000      .00000000D+00 
    1.23E123       .12300000+124     .12300000E+124      ************* 
When the width w is not present in the format specification, the converted 
field contains only the number of characters necessary to express the value of 
the data, plus one blank to the right of the field, as shown in the following 
table: 
Output                 Specification 2E.5        Specification 2E.5E1 
List                   Converts Output List to:  Converts Output List to: 
------                 ------------------------  ------------------------ 
-774.119,1.00001977   -.77412E+03b.10000E+01b    -.77412E+3b.10000E+1b 
The field, counted from the right, includes the exponent digits, the sign 
(minus or plus), the letter E, the magnitude digits, the decimal point, and 
the sign of the value (minus or space).  If a width specification is of 
insufficient magnitude to allow expression of an entire value, the entire 
field is filled with asterisks.  This is not an error condition.  The 
following table illustrates this condition: 
 Value     E11.4       E9.4         E9.4E2     E6.4      E6.4E2 
 -----     -----       ----         ------     ----      ------ 
 -2013.55  -.2014E+04  *********    *********  ******    ****** 
 .361887    .3619E+00  .3619E+00    .3619E+00  ******    ****** 
 .000134    .1340E-03  .1340E-03    .1340E-03  ******    ****** 
To prevent a loss of this kind, it is necessary that the following 
relationships hold true: 
a.  For E Format Normalized, with E Exponent: 
        w > d+6 
    where the value 6 allows for 2 exponent digits, sign of exponent, letter 
    E, decimal point, and sign of value.  The relation must be w > d+7 if a 
    positive scale factor of one is in effect.  (See P_SPEC under the topic 
    PRINT_POSITIONING for an explanation of scale factors.) 
b.  For E Format with Exponent Specifier: 
        w > d+4+e 
    where the value 4 includes character positions for the sign, the decimal 
    point, the exponent character (E or D), and the exponent sign.  A relation 
    of w > d+5+e is required when a positive scale factor of one is in effect. 
Input (E Format) 
INPUT_E_FORMAT 
Input for I, F, and E formats are identical.  See NUMERIC_INPUT_STRINGS under 
the topic FORMATS for a description of the forms permissible for strings of 
input characters.  Input fields for conversion in E format need not have 
exponents specified. 
First, the decimal point is positioned according to the specification; then, 
the value of the exponent is applied to determine the actual position of the 
decimal point.  In the first example, -113409E2 with a specification of E9.6 
is interpreted as -.113409E02, which, when evaluated (i.e., -113409 x 10*2), 
becomes -11.340900. 
Examples: 
    Input Value       Specification       Converted to: 
    -----------       -------------       ------------- 
    -113409E2         E9.6                -11.340900 
    -409385E-03       E.2                 -4.09385 
    849935E-02        E10.5               .0849935 
    6851              E.0                 6851.0 
    -113409E2         E9.6E2              -11.340900 
    -409385E-03       E.2E2               -4.09385 
    848835E-02        E10.5E2             .0849935 
    6851              E.0E3               6851.0 
  D_FORMAT 
  The form is: 
  rDw.d 
  This format is similar to E format, with the exception that, for output, the 
  character D will be present instead of the character E.  For example, 
  whereas: 
    for E12.6, -667.334 is converted to -.667334E+03, 
    for D12.6, -667.334 is converted to -.667334D+03 
Input under D format is the same as for E and F formats. 
G_FORMAT 
There are two G (General) Formats:  (1) G Format; and (2) G Format with 
Exponent Specifier. 
The forms are: 
G Format:                            rGw.d 
G Format with Exponent Specifier:    rGw.dEe 
where 
e    specifies the number of digits in the exponent field.  The 
value of e must be in the range 0<e<9 
These formats are the only formats that may be used with any type of data, 
including logical. The form of conversion performed depends on the type of 
list items. 
For both rGw.d and rGw.dEe specifications, the following table shows the 
equivalent format that is used when processing list items of the various 
types: 
    List Item Type       Input       Output 
    --------------       -----       ------ 
    Integer              Iw          Iw 
    Real                 Fw.d        (see below) 
    Double Precision     Fw.d        (see below) 
    Logical              Lw          Lw 
    Character            Aw          Aw 
Note that, as with all other formats, complex values are processed as two 
separate items; the real and imaginary parts require individual 
specifications, and conversion occurs as shown above for real or double 
precision data. 
For integer, logical and character list items, the d (in Gw.d), and the d and 
e (in Gw.dEe) need not be specified; if present, they are ignored.  This is 
the only case in which d is not assumed to be zero if not specified.  G format 
is very useful in a widthless form.  When so used, the equivalent formats 
shown above become widthless also (see NUMERIC_INPUT_STRINGS under the topic 
FORMATS). 
Output of Real and Double Precision Data - G Format 
OUTPUT_G_FORMAT 
The form of output conversion used with real and double precision values 
depends on the magnitude of the values.  G format attempts to express numbers 
in the most natural way; that is, in F format whenever possible, but in E 
format for values that are too large or too small.  Specifically, d is 
interpreted as indicating the number of significant digits desired, and this 
is exactly the number of digits that are output.  If the value of the number 
is such that it can be expressed by placing the decimal point anywhere within 
or at either end of these d digits, that is what is done, and no exponent is 
appended.  If, however, preceding or trailing zeros are required to express 
the value correctly, F format is not used; instead, the number is normalized 
and output with a following exponent. 
To express this algebraically, let M represent the magnitude of the value to 
be output (rounded to d significant digits).  Select an integer i so that 
    10**(i-1) < M < 10**i     (if M = 0.0, then i = 0) 
For the above integer, the following table compares output conversion for the 
rGw.d and rGw.dEe specifications. 
         rGw.d                                rGw.dEe 
         -----                                ------- 
    Assuming a specification of        Assuming a specification of 
    Gw.d, let n=w-4 and m=d-i.         Gw.dEe, let n=w-2-e, and m=d-i. 
    Then if 0<i<d, the output          Then if 0<i<d, the output 
    conversion takes place             conversion takes place 
    according to the specification     according to the specification 
      Fn.m,4Hbbbb                         Fn.m 2Hbb,e('b') 
    If i is less than 0 or greater     If i is less than 0 or greater 
    than d, the specification          than d, the specification 
    used is                            used is 
      Ew.d                               Ew.dEe 
    When F format is used, four        When F format is used, two plus 
    blanks are output following        e blanks are output following 
    the number in the positions        the number in the positions 
    otherwise containing an            otherwise containing an 
    exponent.                          exponent. 
Note the number of blanks output for each specification.  In this way, the 
numbers that are output in columns tend to line up underneath each other in a 
readable way.  The following examples illustrate the effect of G format output 
on values of various sizes: 
                   G 10.3 or 
     Value         G 10.3E2           G 10.1          G 10.1E1 
     -----         ---------          ------          -------- 
       .02639         .264E-01          .3E-01          .3E-1 
       .2639          .264              .3              .3 
      2.639           2.64              3.              3. 
     26.39            26.4              .3E+02          .3E+2 
    263.9             264.              .3E+03          .3E+3 
   2639.              .264E+04          .3E+04          .3E+4 
The choice of F or E format is independent of the value of the width w.  If w 
is not large enough, the output field is filled with asterisks.  To ensure 
that this does not happen, the following relation should hold true: 
  for rGw.d      w>d+6      and      w>d+7      If a positive scale factor 
                                                of one is in effect. 
  for rGw.dEe    w>d+4+e    and      w>d+5+e    If a positive scale factor 
                                                of one is in effect. 
                                                See P_SPEC under the 
                                                topic PRINT_POSITIONING 
                                                for an explanation 
                                                of scale factors.) 
When no width is specified, the number is followed by a single blank; values 
output in F form are not followed by four blanks. 
Scale factors apply to G format only when the E form is used, not when the F 
form is used.  This has the effect that all values output in G format are 
unchanged (except for rounding).  It also has the effect that values output in 
F form with a P scale factor cannot subsequently be input using the same 
format; the scale factor takes effect during input but not during output. 
Thus, the new value is different from the old by a power of 10. 
Note that the rounding applied to M (above) to determine whether to use E or F 
format is not necessarily the same rounding that is applied when the number is 
actually output.  Consider the following case: 
      PRINT 5,99.76 
    5 FORMAT(1P,G.2) 
In principle, F form is to be used if the value lies in the range .1 < M < 
100.  The value 99.76 does lie in this range, but when rounded to two digits 
it becomes 100., which is outside the range; so E form is used.  First the 
unrounded value of M is normalized (.9976E+02), then the P scale factor is 
applied (9.976E+01), and finally this value is rounded to 9.98E+01, which is 
the way it is printed.  If the first rounding is used throughout, the final 
value will be 1.00E+02, which is less accurate. 
I_FORMAT 
There are two I (Integer) Formats:  (1) I Format; and (2) I Format with 
Specified Minimum Field Width. 
The forms are: 
I Format (Integer):        rIw 
I Format with Minimum Field Width:     rIw.m 
where m specifies the minimum number of digits in the output field.  If 
the value of m is not less than w, m is assumed to be w-1. 
Integer, real, double precision, or either part of complex data may be 
processed by this form of conversion.  If the width specification w is of 
sufficient magnitude, real and double precision values are converted in full 
precision.  In other words, values greater than the maximum permissible size 
of integer data may be processed without the truncation of the most 
significant digits that are normally associated with integer operations. 
Output (I Format) 
OUTPUT_I_FORMAT 
Internal values are converted to integer constants.  Real and double precision 
data are truncated to integer values; however, the integers may contain as 
many digits as are specified by w.  Negative values are preceded by a minus 
sign.  The field is right-justified and preceded by the appropriate number of 
blanks.  The following table gives examples of the conversion of data: 
                            I Format                I Format 
         Value              Specification I6        Specification I6.3 
                            Converts Value to:      Converts Value to: 
         -----              ----------------        ------------------- 
         273.5                 273                       273 
             7                   7                       007 
            -7                  -7                      -007 
             0                   0                       000 
         -.003                   0                       000 
    -44204.965              -44204                    -44204 
          -404                -404                      -404 
Iw.0 produces all blanks for a value of zero. 
Whenever w is unspecified, the converted field occupies the minimum of the 
number of digits required to express the data value and m, plus the sign 
position (if required).  This minimum number of digits is followed by one 
blank, as shown in the following examples: 
The output list 
    345.9, 70000, -2, -.999, 3030.3030 
is converted to: 
    345b70000b-2b0b3030b        by the specification 5I, and to 
    345b70000b-02b00b3030b      by the specification 5I.2 
If the magnitude of data requires more positions than is permitted by the 
value of the width w, the entire field is filled with asterisks.  This is not 
an error condition.  For example, 
    -778801   is converted to  **    by the specification I2, and 
    -778801   is converted to  ****  by the specification I4.2 
Input (I Format) 
INPUT_I_FORMAT 
Input is identical for I, F, and E formats.  See NUMERIC_INPUT_STRINGS under 
the topic FORMATS for a description of the forms permissible for strings of 
input characters.  Conversion for I format is identical to F format 
processing, with the exception that fractional portions of a value are lost 
through truncation.  As noted above, however, the most significant digits will 
not be truncated.  For example, the input field: 
    4570000000000000000000000.942 
processed by an I (widthless) format, into a real or double precision 
variable, produces the internal value 
    4.57 x 10**24 
L_FORMAT 
The form is 
rLw 
Only logical data may be processed with this form of conversion; any other 
data type causes an error to occur. 
Output (L Format) 
OUTPUT_L_FORMAT 
Logical values are converted to either T for "true" or F for "false".  The T 
and F characters are preceded by w-1 blanks.  For examples, using the 
specification L4, 
    .TRUE.   is converted to     bbbT 
    .FALSE.  is converted to     bbbF 
Specifications in which w is unspecified cause the following conversions: 
    .TRUE.   is converted to     Tb 
    .FALSE.  is converted to     Fb 
Input (L Format) 
INPUT_L_FORMAT 
If a width is specified, the first T or F encountered in the next w characters 
determines whether the value is "true" or "false", respectively.  If no T or F 
is found before the end of the field, the value is "false".  Thus a blank 
field has the value "false".  Characters appearing between T or F and the end 
of the field are ignored, including commas.  For example, the following input 
fields, processed by an L7 format, have the indicated values: 
    True     False 
    ----     ----- 
    T        F 
    TRUE     FALSE 
    .TRUE.   .FALSE. 
    RIGHT    READ 
    STAFF    LEFT 
    24T+T42  (blank) 
For widthless logical input, the field terminates at the first comma or 
nonleading blank.  In other words, if the first nonblank character is a comma, 
it terminates the field; if it is not a comma, the next blank or comma 
terminates the field.  The first T or F encountered within the field 
determines the value.  If neither a T nor an F appears, the field has the 
value "false".  As above, characters appearing between the first T or F and 
the blank or comma are ignored. 
A_FORMAT_CHAR_DATA 
The form is 
rAw 
Output (A Format - Character Data) 
OUTPUT_A_NON_CHAR_DATA 
The internal character string in the character datum appears in the output 
field.  If the length n of the character datum is less than w, the leftmost 
w-n characters of the output field are blanks. 
If the length n is greater than w, the rightmost n-w characters of the 
character datum do not appear in the output field. 
When the width is not specified, i.e., the form is rA, the output field is of 
width n and the entire character data appears in the output field.  Note that 
no trailing blank is appended to the output field as is done for widthless 
numeric output. 
Input (A Format - Character Data) 
INPUT_A_FORMAT_CHAR_DATA 
The character string in the input record field, of length 2, defines the 
character datum, of length n, depending upon the relationship of w to n. 
    For w = n, the entire input record field is assigned to the character 
               datum. 
    For w < n, the input record field with n-w trailing blanks is assigned to 
               the character datum. 
    For w > n, the rightmost n characters of the input record field are 
               assigned to the character datum. 
When w is not specified, w is assumed equal to n. 
A_FORMAT_NON_CHAR_DATA 
The form is 
rAw 
Output (A Format - Other than Character Data) 
OUTPUT_A_NON_CHAR_DATA 
Internal binary values are converted to character strings at the rate of one 
byte per character.  The most significant digits are converted first.  That 
is, conversion is from left to right.  The number of characters produced by an 
item depends on the number of words of storage allocated for that type of item 
(See STORAGE_ALLOCATION_STATEMENTS).  Assuming standard size specifications, 
the following examples illustrate the form of A format conversion: 
Data                                                              External 
Type             Internal Binary/Octal                        Aw  String 
----             ---------------------                        --  -------- 
integer,  001 001 001  001 001 110  001 010 100  000 101 010  A4  INT* 
real or    1   1   1    1   1   6    1   2   4    0   5   2 
logical   /---------/  /---------/  /---------/  /---------/ 
               I            N            T            * 
                                                              A6  bbINT* 
                                                              A   INT* 
As with all other format conversions, complex data are treated either as two 
real values or as two double precision values.  In each of the examples above, 
the first A format specifies exactly the number of characters required to 
express the data fully, and therefore has the same effect as the widthless 
form.  Normally, alphanumeric information is used with character data.  In the 
examples, note that when the magnitude of w does not provide for enough 
positions to express the data value completely, the external field is 
shortened from the right (least significant) portion.  This is not treated as 
an error condition.  When w has a value greater than necessary, the external 
character string is preceded by the appropriate number of blank characters. 
When the field width is not specified, the external character string consists 
of only the number of positions necessary to fully express the character value 
of the data.  The external character string is not followed by a blank. 
Alphanumeric conversions are normally used to output Hollerith information 
created as the result of: 
1.  Previous input using an alphanumeric format (A or R). 
2.  Use of a Hollerith constant, e.g., in a DATA statement. 
3.  Use of a literal constant, i.e., in a DATA statement, or passed as an 
    argument. 
This form of conversion is not recommended with random numeric values created 
other than as above, because not all of the 512 characters that can be 
produced can actually be printed.  The nonprintable characters may, however, 
be useful in other contexts (e.g., on cards, or in ENCODE operations). 
Input (A Format - Other than Character Data) 
INPUT_A_FORMAT_NON_CHAR_DATA 
When the width w is larger than necessary (that is, when its magnitude is 
greater than the number of characters associated with the data type of the 
corresponding list item), the list item is filled with the rightmost 
characters.  For example, if the list item is integer, and the specification 
A10 is used, 
    ABCDEFGHIJ      is converted to      GHIJ 
However, when the value of w is less than the number of characters associated 
with the data type of the list item, the most significant positions of the 
list item are filled with w characters, and the remainder of the positions are 
filled with blanks.  Consequently, when the list item is double precision and 
the field specification is A6, 
    UVWXYZ      is converted to      UVWXYZbb 
If the width has a value equal to the number of characters associated with the 
data type of the list item, the list item is completely filled with the 
external field. 
Widthless specifications cause the list item to be filled by the next n 
characters from the input string, where n is the number of characters 
associated with the data type of the list item.  If a list contained 
references to a real variable, an integer variable, and a double precision 
variable, in that order, and a field specification of 3A were used, processing 
would be in the following manner: 
    ABCDEFGHIJKLMNOP 
is converted to: 
    ABCD      EFGH      IJKLMNOP 
A rule for this type of conversion is that internal values are considered to 
be left-justified , while external fields are considered to be 
right-justified. 
R_FORMAT 
The form is 
rRw 
This form of conversion is similar to A conversion.  However, the rule of 
internal justification is reversed from that of the A format specifier:  For 
arithmetic and logical variables, internal values are considered to be 
right-justified with leading binary zeros, whereas with A format they are 
left-justified with trailing ASCII blanks.  For character variables, internal 
values are right-justified with leading blanks. 
Output (R Format) 
OUTPUT_R_FORMAT 
When the size of w is insufficient to allow expression of the complete 
internal value, R format takes characters from the rightmost (least 
significant) portion of the internal value.  In all other respects it is 
identical to A format output.  This difference is illustrated in the examples 
that follow. 
Examples: 
                   Internal 
                   Character 
    Data Type      Value         w      A Format     R Format 
    ---------      --------      -      --------     -------- 
    Integer,       INT*          4      INT*         INT* 
    real, or                     2      IN           T* 
    logical                      6      bbINT*       bbINT* 
                                 None   INT*         INT* 
    Character *8   DOUBLE=2      8      DOUBLE=2     DOUBLE=2 
                                 6      DOUBLE       UBLE=2 
                                 10     bbDOUBLE=2   bbDOUBLE=2 
                                 None   DOUBLE=2     DOUBLE=2 
Input (R Format) 
INPUT_R_FORMAT 
As on output, R format differs from A format only when the specified width (m) 
is less than the number of characters associated with the type of the input 
list item.  In this case, R format fills the least significant (rightmost) 
portion of the list item with w characters from the input string, preceded by 
enough binary zeros or blanks to fill the remaining portion.  In other words, 
R format right-justifies the characters and inserts leading binary zeros or 
blanks, depending on the data type, while A format left-justifies the 
characters and inserts trailing ASCII blanks.  Examples: 
    List Item       External              Internal after      Internal after 
    Data Type       String        w       A Conversion        R Conversion 
    ---------       --------      -       --------------      -------------- 
    Integer, real   XYINT*        4            XYIN                XYIN 
    or logical                    6            INT*                INT* 
                                  2            XYbb                zzXY 
                                  None         XYIN                XYIN 
    Character*8     85DOUBLE=2    8          85DOUBLE            85DOUBLE 
                                  6          85DOUBbb            zz85DOUB 
                                  10         DOUBLE=2            DOUBLE=2 
                                  None       85DOUBLE            85DOUBLE 
where z represents nine binary zeros. 
Note that the zero character is not represented internally as binary zeros. 
Consequently, if the external field 
00ABAB 
is processed by the format specifications A4, R2 into two integer variables, 
the resulting values will be the Hollerith constants, 4H00AB and 2RAB, which 
are not equivalent.  For input as true right-justified integers, R format 
should be used.  (See also CHARACTER_MANIPULATION.) 
Z_FORMAT 
The form is 
rZw 
Z conversion is similar to R conversion, except that the internal data is 
processed four bits at a time instead of eight, and the external field 
consists of the hexadecimal digits, which are: 
    0 1 2 3 4 5 6 7 8 9 A B C D E F 
Note that a list item of type character cannot be the target of an input Z 
format specification. 
Output (Z Format) 
OUTPUT_Z_FORMAT 
Internal binary values are converted to hexadecimal digit strings at the rate 
of 4 bits per digit.  The number of characters produced by an item depends on 
the number of words of storage allocated for that type of item.  (See 
STORAGE_ALLOCATION_STATEMENTS.)  For example, an integer produces 9 digits; a 
double precision number, 18 digits. 
If w as specified is not large enough, the leftmost digits are lost.  If w is 
larger than the number of positions necessary to express the data, the digits 
are right-justified in the field, with preceding blanks.  When field width 
permits, all of the digits in an item are output, including leading zeros. 
When w is not specified, the full number of digits necessary to express the 
value is output followed by a blank.  The blank is to facilitate subsequent 
rereading of the value. 
Examples: 
    Date Type:  Integer, real, or logical 
    ------------------------------------- 
                                                       External 
                Internal Binary/Hexadecimal      Zw     String 
    -------------------------------------------- --    -------- 
    0000 0000 0000 0000 1000 1110 0011 0101 1100 Z8    0008E35C 
       0    0    0    0    8    E    3    5    C 
                                                 Z6    08E35C 
                                                 Z10   b00008E35C 
                                                   Z   00008E35Cb 
    Data Type: Double precision 
    --------------------------- 
                                                       External 
                Internal Binary/Hexadecimal      Zw     String 
    ---------------------------------------      --    -------- 
    1000 0001 0011 0010 0100 0011 1111 0110      Z18  413243F6A8885A300 
       4    1    3    2    4    3    F    6      Z11  6A8885A3000 
    1010 1000 1000 1000 0101 1010 0011 0000      Z20  bb413243F6A885A300 
       A    8    8    8    5    A    3    0      Z    413243F6A885A3000b 
    0000 0000 
       0    0 
Input (Z Format) 
INPUT_Z_FORMAT 
The corresponding list item cannot be of type character. 
When the width w is larger than necessary (i.e., when its magnitude is greater 
than the number of digits associated with the data type of the corresponding 
list item), the list item is filled with the rightmost characters in the 
field. 
When w is too small, the digits are right-justified in the list item, as with 
R format.  When the width exactly corresponds to the number of digits 
associated with the list item, the item is completely filled with the external 
field. 
The significant difference between Z format and R format on input is that Z 
format is a numeric, not an alphanumeric format.  Therefore, commas may be 
used to terminate a hexadecimal input string.  Furthermore, the length of a 
widthless Z input string is not dependent on the size associated with the list 
item; like all other numeric formats, a widthless hexadecimal input string 
terminates at the first comma or nonleading blank.  Excess digits are lost at 
the left.  Note that when w is specified, blanks are treated as zeros.  The 
following are examples of Z format input (assuming an integer list item): 
    External Input Field       Format     Internal Hexadecimal Value 
    --------------------       ------     -------------------------- 
    3A70049B                   Z5          00003A700 
    bbb3A7bb                   Z8          00003A700 
    bD68,47019                 Z8          000000D68 
    DCBA987654321              Z12         A98765432 
    52CA91                     Z           00052CA91 
    123456789ABC,              Z           456789ABC 
    bb49bb3                    Z           000000049 
O_FORMAT 
The form is 
rOw 
O conversion is similar to R conversion, except that the internal data is 
processed three (instead of nine) bits at a time and the external field 
consists of the octal digits, which are: 
    0 1 2 3 4 5 6 7 
Output (O Format) 
OUTPUT_O_FORMAT 
Internal binary values are converted to octal digit strings at the rate of 3 
bits per digit.  The number of characters produced by an item depends on the 
number of words of storage allocated for that type of item (see 
STORAGE_ALLOCATION_STATEMENTS).  For example, an integer produces 12 digits; a 
double precision number, 24 digits. 
If w as specified is not large enough, the leftmost digits are lost.  If w is 
larger than the number of positions necessary to express the data, the digits 
are right-justified in the field, with preceding blanks.  When field width 
permits, all of the digits in an item are output, including leading zeros. 
When w is not specified, the full number of digits necessary to express the 
value is output followed by a blank.  The blank is to facilitate subsequent 
rereading of the value. 
Examples: 
    Data Type: Integer, real, or logical 
    ------------------------------------ 
    Internal Binary/Octal 
    --------------------- 
    000 000 000 000 000 000 000 100 110 011 101 100 
      0   0   0   0   0   0   0   4   6   3   5   4 
    Ow                 External String 
    --                 --------------- 
    O8                 0046354 
    O6                 046354 
    O14                bb000000046354 
    O                  000000046354b 
    Data Type: Double precision 
    --------------------------- 
    Internal Binary/Octal 
    --------------------- 
    100 001 011 010 100 011 111 110 
      4   1   3   2   4   3   7   6 
    010 000 000 000 101 010 011 000 010 011 000 001 100 000 011 101 
      2   0   0   0   5   2   3   0   2   3   0   1   4   0   3   5 
    Ow                 External String 
    --                 --------------- 
    O24                413243762000523023014035 
    O11                23023014035 
    O26                bb413243762000523023014035 
    O                  413243762000523023014035b 
Input (O Format) 
INPUT_O_FORMAT 
The corresponding list item cannot be of type character. 
When the width w is larger than necessary (i.e., when its magnitude is greater 
than the number of digits associated with the data type of the corresponding 
list item), the list item is filled with the rightmost characters in the 
field. 
When w is too small, the digits are right-justified in the list item, as with 
R format.  When the width exactly corresponds to the number of digits 
associated with the list item, the item is completely filled with the external 
field. 
The significant difference between O format and R format on input is that O 
format is a numeric, not an alphanumeric format.  Therefore, commas may be 
used to terminate an octal input string.  Furthermore, the length of a 
widthless O input string is not dependent on the size associated with the list 
item; like all other numeric formats, a widthless octal input string 
terminates at the first comma or nonleading blank.  Excess digits will be lost 
at the left.  Note that when w is specified, blanks are treated as zeros.  The 
following are examples of O format input (assuming an integer list item): 
    External Input Field        Format      Internal Octal Value 
    --------------------        ------      -------------------- 
      35700453                  O5          000000035700 
      bbb357bb                  O8          000000035700 
      b74,47015                 O8          000000000074 
      7654327654321             O12         765432765432 
      523241                    O           000000523241 
      123456765432              O           123456765432 
      bb47bb3                   O           000000000047 
M_FORMAT 
M format is intended primarily for output.  It provides a machine-dependent 
method of dumping information in the format most appropriate to the machine on 
which the program is running.  Thus, on an octal machine it is interpreted as 
O format, and on a character machine, as A format.  On the CP-6 operating 
system it is interpreted exactly the same as O (octal) format.  Thus it can 
also be used for input, though this is not recommended. 
H_FORMAT 
The form is 
nHs 
where n<511 
Output (H Format) 
OUTPUT_H_FORMAT 
The n characters in the string s are transmitted to the external record. 
Examples: 
    Specification      External String 
    -------------      --------------- 
    1HE                E 
    8HbbVALUE:         bbVALUE: 
    5HS3.95            S3.95 
    9HX(2,5)b=b        X(2,5)b=b 
Care should be taken that the character string s contains exactly n 
characters, so that the desired external field is created and characters from 
other format specifications are not used as part of the string. 
Input (H Format) 
INPUT_H_FORMAT 
The n characters in the string s are replaced by the next n characters from 
the input record. This replacement occurs as shown in the following examples: 
    Specification      Input String      Resultant Specification 
    -------------      ------------      ----------------------- 
    3H123              ABC               3HABC 
    1OHNOWb1SbTHE      bTIMEbFORb        10HbTIMEbFORb 
    5HTRUEb            FALSE             5HFALSE 
    6Hbbbbbb           RANDOM            6HRANDOM 
This feature can be used to change the titles, dates, column headings, etc., 
that are to appear on an output record generated by the H specification.  Use 
of the INPUT feature of the Hollerith format specifier is not recommended. 
This feature is provided for backward compatibility with previous FORTRAN 
processors.  Character variables, using the 'A' format specifier should be 
used for variable print information. 
INPUT_CHAR_FIELD_DESC 
The modification of character field descriptors during input is permitted only 
in FORMAT statements.  It is not permitted in FORMATs stored in arrays or 
character variables or character expressions.  Programs using this feature may 
not execute properly in an overlay environment.  Use of this feature is not 
recommended.  It is provided only for compatibility with prior FORTRAN 
systems. 
It should be noted that in order for the inputting of data into a string field 
to work, the NMP (No Memory Protection) Compiler option must be specified. 
NUMERIC_INPUT_STRINGS 
The permissible kinds of input strings that may be processed by numeric 
conversions are exactly the same for F, E, D, G, and I conversion.  Any field 
that can be read using one of these formats can be read using any of the 
others.  In other words, numbers for input with E format need not have 
exponents, numbers for input with I format need not be integers, etc. 
A numeric input string consists of a string of digits with or without a 
leading sign, a decimal point, and/or a trailing exponent.  An exponent is 
normally specified as: 
E+e 
where the plus sign is optional and e is a one or two-digit number.  The form 
+e is also accepted (without the E), in which case the plus sign is not 
optional.  Thus, a variety of forms may be used to express data for numeric 
input. 
    +n      +n.m      +n.      +.m 
    +nE+e   +n.mE+e   +n.E+e   +.mE+e 
    +n+e    +n.m+e    +n.+e    +.m+e 
where the plus signs are optional except in an exponent field without an E (as 
described above). 
When input fields contain no decimal point (as in the first column above) the 
decimal point is positioned according to the d in the format specification (as 
in Ew.d).  If none is specified it is assumed to be zero.  The decimal point 
is placed d positions to the left of the beginning of the exponent, or if no 
exponent is present, d positions to the left of the end of the field.  Note 
that the exponent may begin with either a D,E,+ or -. 
A D may be substituted for the E in an exponent field, with no change in 
meaning or value.  It is not necessary to indicate that data is double 
precision, nor is it necessary to use a D format.  Regardless of the format 
used or the form of exponent (if any), a numeric string is converted with full 
double precision if the input list item to which it is to be assigned is 
double precision. 
Any numeric type of list item may be used with any numeric type of format 
specification.  If the list item is integer, the input value is processed in 
floating-point, if necessary, and then converted to integer.  When the I 
format specification is used (with any type of list item), the fractional 
portion of the value is lost. 
A comma may be used to terminate any numeric field, as described below. 
Leading blanks are always ignored.  The interpretation of embedded and 
trailing blanks depends on whether or not the format specification used is 
widthless (no width specified). 
WIDTHLESS_NUMERIC_INPUT 
The principle behind widthless input is that the field ends when the number is 
finished.  A comma always indicates that the number is finished.  A blank also 
indicates that the number is finished during widthless numeric input, if it is 
meaningful to finish the number at that point.  Thus, 
1.  Leading blanks do not cause termination; they are ignored. 
2.  Any number of blanks may appear in the following places: 
    a.  Between the leading plus or minus sign and the first digit. 
    b.  Between the E and the plus or minus sign or first digit of the 
        exponent. 
    c.  Between the plus or minus sign in the exponent and the first digit of 
        the exponent. 
3.  A blank that follows a digit or decimal point terminates a widthless 
    numeric input field. 
4.  When a widthless (or any other) field is terminated by the end of the 
    input record, the extra characters are interpreted as blanks.  Normally, a 
    widthless format does not terminate until at least one nonblank character 
    has been found.  Special provision is made, however, to terminate 
    widthless fields at the end of the record.  Thus, any number of numeric 
    values may be read from a blank record, and they will all be zero. 
5.  The BN and BZ specifications have no effect on widthless numeric input. 
For clarity, numbers should generally be written without any embedded blanks. 
The first blank terminates the field.  Although the terminating blank or comma 
does not affect the value of the number, it is considered part of the field it 
has terminated.  Therefore, the next field begins with the character following 
the blank or comma. 
The following is a typical widthless numeric input line consisting of eight 
values: 
    73    2E-4     .0007     -35.4     0     0     -16     27.08614E12 
The following is not a typical widthless numeric input line: 
    -   3E   +   2   +   3.7   -   4   17E   2   5-   03 
but would be interpreted as five values, namely 
    -300.   3.7   -4.   1700.   .005 
NUMERIC_INPUT_WIDTH_SPECIFIED 
When a width is specified, the field terminates only when the width is 
exhausted or a comma is found.  The following rules apply to blanks in numeric 
fields with a width specified: 
1.  Leading blanks are ignored, except that they are counted as part of the 
    field width. 
2.  Once any nonblank character has been found, all blanks beyond that point 
    are treated as zero or null, depending upon the BLANK=setting for the 
    unit.  (See OPEN.) 
3.  Any string of digits that is omitted has an assumed value of zero. 
For a format specification such as F10.0, with no P scale factor, all the 
input strings in each of the columns below produce the value shown in the top 
line of the column.  The first three lines in each column are typical numeric 
fields; the others are permissible, but less readable. 
    ------------------------------ 
         -.004    7.5E12         0 
    ------------------------------ 
         - E-3   .75D+13       0.0 
    ------------------------------ 
    -.004         750E10 
    ------------------------------ 
      -  4 - 4    75 E1   0  +  0 
    ------------------------------ 
    -  .  4D    75E+ 01    0E 
    ------------------------------ 
     -4     -8  .  75E16  +    - 
    ------------------------------ 
Note, in the fourth example of the middle column above, that the exponent is 
interpreted as 10 rather than as 1, because the trailing blank is equivalent 
to a zero.  Care should always be taken to assure that exponents are 
right-justified in their fields.  Failure to do this is a common pitfall that 
can also be avoided by using comma termination and/or widthless formats and/or 
BLANK='NULL' in the OPEN statement and/or the BN FORMAT specification. 
COMMA_FIELD_TERMINATION 
Input strings being processed under the control of F, E, D, G, or I 
specification may be terminated at any point by the presence of a comma in the 
string.  For consistency with symbolic input (via the INPUT statement), the 
characters semicolon, asterisk, and right parenthesis are also accepted as 
field terminators.  Use of the comma is recommended, however.  In other words, 
whenever a comma appears in such an input string, the field currently being 
processed is considered ended, and no additional characters are converted. 
This termination occurs regardless of the value of w in the field 
specification.  The comma is not processed, and the next field begins with the 
character following the comma.  For example, the specification BN,2F13.3 and 
the string 
    3450,88412 
is equivalent to F4.3,F5.3 and 
    345088412 
The string containing the comma will also be correctly processed by the 
specification 2F.3 or BN,2F8.3,BZ. 
Two contiguous comma characters indicate an empty field, which has the value 
zero.  Therefore, for the specification 5I6, the string 
    303,,-1,,000450 
is converted to the values 
    303   0   -1   0   450 
and the string 
    0,,,,, 
is converted to the values 
    0   0   0   0   0 
The comma must, of course, fall within the field it is meant to terminate. 
For example, if the format specification F4.0 is used to process the input 
string 
    1234, 
the value will already be terminated because of field width, and the comma 
terminates the following numeric field, giving it a value of zero. 
FORMATS_STORED_IN_ARRAYS 
A FORMAT, including the beginning left parenthesis, the final right 
parenthesis, and the specifications enclosed therein, may be stored in an 
array or a character variable.  If the array is of type other than character, 
the FORMAT must be stored as a Hollerith string (i.e., a string of 
characters), usually by use of an INPUT statement or a DATA statement. 
READ or WRITE statements that refer to a FORMAT stored in an array must 
reference only the identifier of the array, with no subscripts.  For example, 
when R is an array, 
    WRITE (4,R)E,F,G 
refers to a FORMAT stored in an array R, while 
    WRITE (4,R(1))E,F,G 
refers to a FORMAT statement whose label has been ASSIGNed to R(1) when R is 
of type other than character. 
INTEGER_ARRAYS 
If the variable M is an integer array, the following are two methods that may 
be used to store a FORMAT in M: 
  the external string         }                        {  the statement 
                              }                        { 
     (F8.5,4HNAIL,I3)         }                        {  DATA M(1)/4H(F8./ 
                              }                        { 
  and the statements          }   are equivalent to    {  DATA M(2)/4H5,4H/ 
                              }                        { 
     READ (N,90)(M(I),I=1,4)  }                        {  DATA M(3)/4HNAIL/ 
                              }                        { 
  90 FORMAT (4A4)             }                        {  DATA M(4)/4H,I3/ 
Alternatively, M can be a dummy array corresponding to a literal constant 
argument (see ARGUMENTS_DUMMIES). 
Care must be taken when creating values for an array that will represent a 
FORMAT containing specifications of the nHs, $s$, and 's' forms.  In these 
cases, all characters in the string s, including blank characters, are 
significant, while blank characters are insignificant in all other 
specifications.  For example, if M in the above READ statement is double 
precision instead of integer, the following results occur: 
    Element      Stored after READ 
    -------      ----------------- 
    M(1)         (F8.bbbb 
    M(2)         5,4Hbbbb 
    M(3)         NAILbbbb 
    M(4)         ,I3)bbbb 
which is not the desired result, since it is equivalent to the FORMAT 
    (F8.5,4Hbbbb,NA,I,L,I3) 
Even though a FORMAT may be quite short, such as 
    (I8) 
it must be stored in an array.  It may not be stored in a variable of type 
other than character, since a reference to that variable (or an array element) 
is treated as though the variable were assigned the location of a FORMAT 
statement, rather than as if that variable contained the FORMAT. 
FORMATs stored in arrays may be used by all statements that reference a FORMAT 
statement.  For example, 
    PRINT M,A,SQRT(A) 
    DECODE (80,M,BUFFER),(A(J),J=1,10) 
00042
00017
00020
00041
00008
00025
00005
00042
00017
00020
00008
00005
00002
00009
00034
00027
00011
00014
00033
00030
00023
00027
00002
00009
00027
00034
00019
00022
00037
00007
00004
00036
00013
00016
00032
00026
00038
00029
00043
00011
00014
00033
00038
00039
00040
00018
00021
00006
00003
00010
00035
00012
00015
00031
00024
00028
00030
00001
00023
00041
00039
00027
Syntax: 
DATA s1,s2,s3,...,sn 
Parameters: 
si    is a data set specification of the form 
    variable-list/constant-list/ 
The commas separating the si are optional. 
Description: 
Giving a variable or an array element a value with a DATA statement differs 
from doing so with an assignment statement.  With the DATA statement, the 
value is assigned when the program is loaded; with the assignment statement, 
this is done at execution time. 
Consider an example that profits from the use of the DATA statement:  an 
ARCTAN function can be written using a power series expansion.  The efficient 
way to program this in FORTRAN is with a DO or REPEAT loop, stepping through 
the constants.  But constants cannot be subscripted, and the timing of the 
routine is adversely affected if an array must be initialized each time into 
the routine using assignment statements, such as 
    C(0)=0 
    C(1)=.1243549945 
    C(2)=.2447786631 
    etc. 
Here, the DATA statement can be used to great advantage.  It is not 
recommended that the DATA statement be used to give "initial" values to 
variables that are going to be changed.  This causes proper initialization of 
the program to depend on loading and disallows restarting the program once it 
has changed these values.  Good programming practice dictates that such 
initialization be done with executable statements, e.g., with assignment 
statements. 
The effect of the DATA statement is to initialize the variables in each data 
set to the values of the constants in the set, in the order listed.  Variable 
lists and constants lists in DATA statements may be constructed in very 
flexible ways, as described under DATA_VARIABLE_LIST and DATA_CONSTANT_LIST. 
Examples: 
    DATA X,A,L/3.5,7, .TRUE. /,ALPHA/0/ 
is equivalent to the assignment statements 
    X=3.5 
    A=7 
    L= .TRUE. 
    ALPHA=0 
except that the DATA statement is not executable; its assignments take place 
upon loading. 
00002
00003
00001
All data in a FORTRAN-77 program unit belong to one of the eight data types 
listed below: 
  Type       Characteristics and Value Limits 
Integer    Precise representations of the range of integers from 
           -34,359,738,368 to +34,359,738,367; that is, -(2**(35)) to 
           +(2**(35)-1).  Integer data may be assigned only integral 
           values, and only within this range. 
Integer*2  Precise representations of the range of half-word integers 
           from -131072 to +131071; that is, -(2**(17)) to +(2**(17)-1). 
Real       Also known as floating-point data.  Can be assigned 
           approximations of real numbers with magnitudes within the 
           range (4.661463)*(10**(-156)) to (8.379879)*(10**152)). 
           A real datum may acquire any positive or negative value 
           within this range, or the value zero.  Real data have an 
           associated precision of 7+ significant digits.  That is, 
           the seventh most significant digit will be accurate, 
           where as the eighth may or may not be accurate 
           depending on the value assigned to the datum. 
Double     Approximations of the identical set of values as that of 
Precision  real data but with an associated precision 
           of 18+ significant digits. 
Complex    Approximations of complex numbers, taking the form of an 
           ordered pair of real data.  The first part of the 
           pair approximates the real, the second part approximates 
           the imaginary part of the complex number.  The values 
           that may be assigned to each part of the pair are 
           identical to the set of values for real data. 
Double     Like complex data except that both the real and imaginary 
Precision  parts are double precision values. 
Complex 
Logical    Can acquire only the values "true" or "false". 
Character  Character strings of any length up to a maximum of 131,071 
           characters.  Any character within the allowed character 
           set may appear.  The length of a character constant 
           is the number of characters in the string.  A character 
           datum is not arithmetic. 
Literal    Same as character except that length of string is specified. 
A DATA variable list is similar to an input list in that it may contain 
variables, subscripted or unsubscripted arrays, and implied DO loops.  It may 
not contain expressions (except as subscripts and DO parameters) or function 
references.  When no implied DO loops are present, subscripts must be composed 
of constants, as described in rule 2, below. 
The following rules apply to DATA variable lists with implied DO loops: 
1.  The control variable in an implied DO loop (i.e., the v in v=e1,e2,e3) 
    must be an integer variable. 
2.  Subscripted expressions may be composed of integers, DO control variables, 
    parentheses, and the operators +, -, *, and /. 
3.  If a subscript expression contains a variable (see rule 2), the variable 
    must control a loop in which the expression appears. 
4.  The ei used as DO parameters are formed in the same manner as subscript 
    expressions (see rules 2 and 3). 
5.  As implied by rules 3 and 4, the ei parameters in an outermost DO 
    implication must not contain variables.  Similarly, as implied by rule 3, 
    a subscript expression that is not within any implied DO loop may not 
    contain variables. 
6.  An implied DO loop controlled by the variable v may not contain another 
    implied DO loop controlled by the same variable v. 
7.  The control variable v may not appear in the data variable list except in 
    a subscript expression or in a DO parameter expression. The control 
    variable is not assigned a data value by the DATA statement. 
Declaration statements are used to declare the data type of variables, arrays, 
and functions, specify the dimension of arrays, specify storage allocation, 
and define initial values of variables and arrays. 
The following declaration statements and related concepts are discussed here: 
    CONSTANT_STATEMENT 
    PARAMETER_STATEMENT 
    ARRAY_DECLARATORS 
    DIMENSION 
    IMPLICIT 
    EXPLICIT_TYPE_STATEMENTS 
    COMMON_STATEMENT 
    LABELED_COMMON 
    BLANK_COMMON 
    GLOBAL 
    EQUIVALENCE 
    SAVE 
    EXTERNAL 
    INTRINSIC_STATEMENT 
    DATA_STATEMENT 
    NAMELIST_STATEMENT 
    VIRTUAL_STATEMENT 
Syntax: 
DECODE (c,f,s[,n])[,][k] 
Parameters: 
c    defines the number of characters per internal record.  It is an 
expression of integer, real, or double precision type, truncated to integer if 
necessary. 
f    specifies a FORMAT statement.  It may be the statement label of a FORMAT 
statement, a variable that has had such a statement label ASSIGNed to it, or 
the name of an array in which a FORMAT has been stored. 
s    indicates the first element or starting location of the internal buffer. 
It may be an array name, an array element, or a variable name.  Any data type 
is acceptable; character is recommended. 
n    is an optional integer variable into which, upon completion of the 
operation, the number of characters actually processed (generated or scanned) 
will be stored. 
k    is an input/output list. 
Description: 
The DECODE statement causes the character string beginning at location s to be 
decoded according to the FORMAT specified by f, and stored into the items in 
the list k. 
If the FORMAT statement requires more characters from a record than are 
specified by the count (c), the extra characters are assumed to be blanks; 
they are not obtained from the next record.  A new record is begun only when 
specifically requested by the FORMAT (see LIST_INTERFACE under the topic 
PRINT_POSITIONING). 
If n is specified, it will be set to the number of characters scanned.  When 
scanning with widthless formats, this can be very useful.  The following 
example makes use of this feature. 
Examples: 
     CHARACTER KODE*4,KEY*4,KARD*80 
     DATA KEY/'MINE'/ 
     READ 4,KARD 
  4  FORMAT(A80) 
     DECODE(80,5,KARD,NC)KODE,J 
  5  FORMAT(A4,I) 
     IF (KODE.EQ.'MINE'.OR.KODE.EQ.KEY) DECODE(80,6,KARD)*NC,J,(A(I),I=1,J) 
  6  FORMAT(NX,NF) 
     . 
     . 
     . 
The above statements could be used to read records of the form 
    MINE 2,1.75,80.91 
Note that in the above example the first DECODE statement is used to decide 
how to interpret the rest of the card.  DECODE essentially provides the 
capability of "processing the card twice."  ENCODE cannot be used in quite the 
same way because it initially fills its buffer with blanks. 
00002
00003
00001
DECODE
    In the absence of options in the calling sequence for a compilation, the 
    compiler will assume the following: 
    OU, OP, BC(ALL)  (in interactive use with a program source file, or with a 
    batch job) 
        or 
    OU, OP, BC(1)    (in interactive use without a program source file) 
If any options are specified in the calling sequence (including the implicit 
options from the invocation line), only those options should be considered 
active, with the exception of OU, OP, and BC as specified above. For example: 
    !FORTRAN FILE_X OVER *G 
sets the options OU, OP, and BC(ALL) (the defaults assumed by the compiler) 
since a program source file (FILE_X) is specified. 
Note that the default option OU will result in the compiler producing an 
object unit on the OU file.  If no assignment has been made to an OU file, the 
default name is *G.  It should be noted that without an OU file assignment, 
the compiler writes into *G (automatically extending the file), not on it. 
Note also that *G is a system generated temporary file.  It disappears when 
the current session is closed. 
DEFAULT_UNIT_SPECIFIER 
The use of a default specifier for a READ/WRITE statement is specified by the 
use of an asterisk (*) in the unit specifier field of the READ/WRITE 
statement. 
This will cause the I/O operation to automatically default to the default 
input/output unit number for the specific processor.  For FORTRAN-77 the 
defaults are: 
     105 for input 
     108 for output 
or if the US option is selected: 
     5 for input 
     6 for output 
Some forms of the use of the default unit specifier are: 
     READ (UNIT=*,FMT=10,END=21)A,B 
     READ(*,FMT=11)I 
     WRITE(UNIT=*,FMT=12)A,B 
     WRITE(*,FMT=33,ERR=21) 
Syntax: 
DEFINE FILE u1(m1,r1,f1,v1),u2(m2,r2,f2,v2),...,un(mn,rn,fn,vn) 
Parameters: 
u    is the identifier of a logical unit and must be an integer constant. 
m    specifies the number of records in the file associated with the logical 
unit (u).  It must be an integer constant. 
r    specifies the maximum size of each record associated with the logical 
unit.  It must be an integer constant.  The record length is measured either 
in bytes or words, depending on "f". 
f    specifies the READ/WRITE format as one of three letters:  L, E, or U. 
The specification for f may differ for each logical unit described in a DEFINE 
FILE statement. 
    L    specifies that record size shall be measured in bytes, with or 
        without format control. 
    E    specifies format control in accordance with the FORMAT statement, 
        with record size measured in bytes. 
    U    specifies no format control, with record size measured in words 
        (unformatted). 
v    is an integer variable (called the "associated variable") which, at the 
conclusion of each READ/WRITE operation, is set to a value that points to the 
record immediately following the last record transmitted.  At the conclusion 
of a FIND operation, the value of v points to the record found. 
Description: 
The DEFINE FILE statement describes the characteristics of one or more data 
files, each associated with a logical unit that is identified and referred to 
by an integer number.  Format control options are provided in the DEFINE FILE 
statement. 
The number of the next record following the one being processed is made 
available to the program via the associated variable, a pointer that assures 
automatic sequential processing following a randomly selected record.  The 
associated variable is specified by name in the DEFINE FILE statement. 
In the execution sequence, the DEFINE FILE statement must precede any READ, 
WRITE, or FIND statement that references that logical unit to describe the 
logical unit as a DEFINEd FILE.  The logical unit may be defined only once per 
program or subprogram.  Any subsequent definition of the logical unit will be 
ignored. 
Examples: 
    DEFINE FILE 10(500,100,L,IK),20(1000,75,L,JK) 
This statement defines two files represented by logical units 10 and 20.  The 
file represented by unit 10 may consist of 500 records, each with a maximum 
length of 100 bytes.  Measurement in bytes is implied by L, which also 
specifies that data may be transmitted with or without format control.  IK is 
the associated variable that serves as a pointer to the next record. 
The file represented by logical unit 20 may consist of 1000 records, each with 
a maximum size of 75 bytes.  L is the same as before, and JK is the associated 
variable.  If E were substituted for L in either of the unit definitions, then 
the data for that logical unit would have to be transmitted under format 
control.  That is, a FORMAT statement would be required in the READ and/or 
WRITE statements.  Similarly, if U were to be substituted, no format control 
would be assumed and the maximum size of each record would be in words instead 
of bytes. 
00002
00003
00001
DEFINE_FILE
ARRAY_BLOCK_DESIGNATOR
Syntax: 
DIMENSION vi,v2,v3,...,vn 
Parameters: 
vi    are array declarators. 
Description: 
This statement is used only to define the dimensions of an array.  A DIMENSION 
statement does not affect the type of allocation of the arrays declared. 
Examples: 
    DIMENSION MGO(19),LTO(16),BB(36,22,34),ZQ9(IB,IC) 
    DIMENSION AD(0:1984),X(2,3,4,5:10),PERTOL(5,6) 
00002
00003
00001
DIMENSION_STATEMENT
Syntax: 
DO [k[,]] [v=e1,e2[,e3]] 
Parameters: 
k    is a statement label. 
v    is a variable of integer, real, or double precision data type. 
ei    are integer, real or double precision expressions. 
Description: 
A DO statement indicates that the block of statements following it are to be 
executed repetitively.  Such a block is called a DO loop, and all statements 
within it, except for the opening DO statement, constitute the range of the DO 
statement.  The last statement in a DO loop is the terminus and bears the 
statement label k.  If the terminal statement of a DO is a compound statement, 
only the first element of the compound statement is included in the DO loop 
(see COMPOUND_STATEMENTS). 
If the terminus statement label, k, is not specified, the END DO statement 
must be used to terminate the DO loop.  If the DO statement does not contain 
the DO control expression, the DO loop is an infinite loop and can only be 
exited by use of the EXIT statement or a GO TO statement.  This type of loop 
must have an END DO statement as the DO loop terminus statement. 
The execution of a DO loop proceeds in the following steps: 
1.  The expression e1, e2, and e3 are evaluated and converted to the mode of 
    v, if necessary; these values are used throughout the execution of the DO 
    range.  If e3 is not present, the value 1 is used.  The expressions e1 and 
    e2 must appear.  The value of e3 must not be zero. 
2.  The variable v is assigned the value of e1. 
3.  The iteration count is defined.  It is the largest integer value that does 
    not exceed the value of the expression 
        (e2-e1+e3)/e3 
    If the value of the expression is less than zero, the iteration count is 
    defined to be zero. 
4.  If the iteration count is zero, the DO loop is terminated and the 
    execution continues subsequent to the statement with the label k. 
    If another outer DO loop also terminates on the statement with the label 
    k, execution continues at that DO terminus; otherwise, execution continues 
    with the next executable statement following the statement with the label 
    k. 
    Some FORTRAN implementations always execute statements within a DO range 
    at least once, regardless of the iteration count.  In FORTRAN-77, this 
    type of DO loop code may be generated by compiling with the OTD (One Trip 
    DO) option.  For more information, enter COMPILER_OPTIONS OTD. 
5.  If iteration count is positive, the normal execution of the DO range 
    proceeds. 
6.  One iteration is complete when this normal execution sequence reaches the 
    statement with the label k. 
If the innermost DO loop of a DO nest that has the statement with the label k 
as the terminal statement has an iteration count of zero (i.e., is inactive), 
the statement with the label k is treated as a CONTINUE statement. 
After each iteration, the value of v is incremented by the value of e3, the 
iteration count is decremented by one, and the process is continued at step 5, 
above. 
The terminal statement of a DO range (i.e., the statement whose label is k) 
may be any executable statement other than one of the following: 
    DO statement 
    REPEAT statement 
    GO TO statement 
    Arithmetic IF statement 
    RETURN statement 
    STOP statement 
    END statement 
    Block IF statement 
    ELSE IF statement 
    ELSE statement 
    END IF statement 
Note that logical IF statements are specifically allowed as terminal 
statements of a DO range, even when they control one of the above statements. 
The value of the variable v appearing in a DO statement depends on the number 
of iterations completed.  When the data type of v is integer, the value of v 
during any one iteration is 
    e1+(i-1)*e3 
i    is the number of the current iteration, and e1 and e3 have the meanings 
discussed above. 
If a transfer is made out of the range of a DO before all iterations have been 
completed, the value of v will be that of the iteration during which the 
transfer occurred.  However, should the entire number of iterations be 
executed, the value of v is 
    e1 + n*e3 
n    is the total number of iterations specified by the DO statement. 
Thus, if all iterations are completed in the example beginning with the 
statement with the label 21, the statement with the label 12 will be 
equivalent to 
    12 L=Y(16) 
However, if the arithmetic IF statement causes a transfer to the statement 
with the label 12 during the eighth iteration, the statement will be 
equivalent to 
    12 L=Y(8) 
The value of the variable v may not be modified within the range of the DO, 
nor may it be modified by a subprogram called within the range of the DO. 
A transfer into the range of a DO from outside the range is not permitted. 
Examples: 
    21 SUM=0.0                                              DO I=J/K,M,N 
    22 DO 54 I=1,15                   DO 30,V=J*2,M/5+D     CALL EVAL(I) 
    23 SUM=SUM+Q(I)                   REG=V+Q(V)**L         IF(I .GT. KK)EXIT 
       IF (SUM .LT. 0.0)SUM=0.0       CALL MAP(TRV+B,REG)   . 
       SIGMA=SUM+R(I)              30 CALL EVAL(H(V))       . 
       IF (SIGMA-H**3/T)54,54,12   .  .                     . 
                                   .  .                     ENDDO 
    54 CONTINUE                    .  . 
    12 L=Y(I)                          DO 9970 Y=99,L,-1.0 
                                  9970 TS(M)=Y-SQRT(R(M)) 
In the example that begins with the statement with the label 21, the range of 
statements 23 through 54 will be executed 15 times, unless the arithmetic IF 
statement causes a transfer to the statement with the label 12.  If all 15 
iterations are completed, control is passed to the statement with the label 12 
following the fifteenth iteration. 
00002
00003
00001
In order to make use of Bull doubleword instructions, the FORTRAN system must 
guarantee that all doubleword quantities begin at an even location.  The 
compiler automatically allocates constants properly, and will always do the 
same for variables in the absence of storage allocation statements. 
When using COMMON and EQUIVALENCE statements, care must be taken not to force 
double precision, complex, or double precision complex variables into odd 
locations.  In particular, this means that 
1.  No doubleword variable should begin at an odd location in a COMMON area. 
    For example, the statements 
        INTEGER J 
        DOUBLE PRECISION D 
        COMMON J, D 
    force D into location 00001 of blank COMMON.  In cases like this, the 
    compiler issues a diagnostic and then moves variables in COMMON to 
    compensate. 
2.  No EQUIVALENCE should specify a doubleword variable located an odd number 
    of words away from any other doubleword variable.  For example: 
        INTEGER M(10) 
        COMPLEX C1, C2 
        EQUIVALENCE (C1,M(1)),(C2,M(4)) 
    The compiler diagnoses and discards such an EQUIVALENCE. 
A DO-implied list is a simple list followed by a comma and then by a 
DO-control of the form: 
v=e1,e2[,e3] 
where 
v    is a DO-control variable which must be an integer, real, or double 
precision variable. 
ei    are DO-parameter expressions of integer, real, or double precision type. 
The meaning of a DO control is similar to that of a DO statement (see below); 
that is, all of the items in the simple list preceding the DO control are 
repeated over and over while v is incremented from e1 to e2 in steps of e3. 
A DO-implied list enclosed in parentheses becomes a simple list item.  Thus, 
an input/output list may contain any number of nested DO-implied lists, with 
the provision that all nested lists (except the outermost list) must be 
enclosed in parentheses. 
Examples: 
    DO-implied List               Equivalent Simple Lists 
    ---------------               ----------------------- 
    X(I),I=1,4                    X(1),X(2),X(3),X(4) 
    A(J),B(J),J=1,3               A(1),B(1),A(2),B(2),A(3),B(3) 
    (G(2*N),N=4,0,-2)             G(8),G(4),G(0) 
    T,(C(J),J=3,5),E,LENGTH       T,C(3),C(4),C(5),E,LENGTH 
    (A(I,J),I=7,9),J=1,3          A(7,1),A(8,1),A(9,1),A(7,2), 
                                    A(8,2),A(9,2),A(7,3),A(8,3), 
                                    A(9,3) 
    SQRT(X),X=0,N,0.5             SQRT(0.0),SQRT(.5),SQRT(1.0), 
                                    ...,SQRT(N-.5),SQRT(N) 
    R,T(K),K=2,3                  R,T(2),R,T(3) 
    F(C),C=5*T,B,J+K              F(5*T),F(5*T+J+K),T(5*T+2*(J+K)),etc. 
The DO-control variable in an input/output list is available and may also be 
used as a list item.  However, in an input list, the DO-control variable must 
not be modified.  The output list 
    (K,A(K),K=1,3) 
is equivalent to the simple list 
    1,A(1),2,A(2),3,A(3) 
In FORTRAN-77, the DO-control of a DO-implied list functions the same way as a 
DO statement.  In other words, the expressions e1, e2, and e3 are evaluated 
the first time through the loop and the loop count is computed.  If the 
terminal conditions of a loop are met initially, it will be done "no times". 
Thus, the lists 
    G(K),K=34,22 
    (J,X(J),J=1,10,-1) 
produce no list elements at all. 
In a DO control, if any of the expressions e1, e2, or e3 are of a different 
data type than the control variable, they are evaluated and then converted to 
the data type of the variable.  Therefore, the list 
    (A(J),J=-3,-1.5) corresponds to the simple list 
    A(-3),A(-2),A(-1) 
since -1.5 becomes -1 when truncated to an integer. 
00001
DO
Syntax: 
DO (e) TIMES 
Parameters: 
e    is as integer extension. 
TIMES    is a textual keyword. 
Description: 
The DO TIMES statement specifies that all statements following it, prior to 
the next END DO statement, are to be executed repetitively.  The number of 
times the loop is to be executed is specified by "e".  The integer expression 
"e" must be a positive value.  If it is not a positive value, the loop will 
not be executed. 
The DO TIMES statement is similar to the DO statement.  The differences 
between them are as follows: 
1.  The DO TIMES statement does not have a DO loop control variable. 
2.  The DO TIMES statement iterates a fixed number of times. 
3.  The loop framed by the DO TIMES statement must be terminated by an END DO 
    statement. 
Example: 
    DO (50) TIMES 
        I = I - J 
        CALL SS(I,J) 
    END DO 
00002
00003
00001
DO_TIMES
Syntax: 
DO [s[,]] WHILE (e) 
Parameters: 
s    is the label of an executable statement that must physically follow in 
the same program unit. 
e    is a logical expression. 
Description: 
A DO WHILE statement tests the logical expression at the beginning of each 
execution of the loop, including the first.  If the value of the expression is 
true, the statements in the body of the loop are executed; if the expression 
is false, control transfers to the statement following the loop. 
If no label is specified for the DO WHILE statement, the loop must be 
terminated with an END DO statement (see ENDDO_STATEMENT). 
Example: 
    CHARACTER*132 LINE 
    I = 1 
    LINE(132:) = 'x' 
    DO WHILE (LINE(I:I) .EQ. ' ') 
        I = I + 1 
    END DO 
Control can be transferred out of a DO WHILE loop, but not into a 
loop from elsewhere in the program. 
00002
00003
00001
DO_WHILE
A dummy argument may be defined to be an array by the presence of its 
identifier in an array declarator within the subprogram.  (The fact that a 
calling argument is an array does not in itself define the corresponding dummy 
to be an array.)  As with all dummies, a dummy array does not actually occupy 
any storage; it merely identifies an area in the calling program.  The 
subprogram assumes that the argument supplied in the calling statement defines 
the first (or base) element of an actual array and calculates subscripts from 
that location. 
Normally, a dummy array is given the same dimensions as the argument array to 
which it corresponds.  This is not necessary, however, and sometimes useful 
operations can be performed by making them different.  For example, 
    DIMENSION A(10,10)       SUBROUTINE OUT(B) 
    CALL OUT (A(1,6))        DIMENSION B(50) 
    . 
    . 
    . 
In this case, the one-dimensional dummy array B corresponds to the last half 
of the two-dimensional array A (i.e., elements A(1,6) through A(10,10). 
However, since an array name used without subscripts as an argument refers to 
the first element of the array, if the calling statement is: 
    CALL OUT(A) 
the dummy array B will correspond to the first half of the array A. 
Actual arguments that are character constants are normally received by a dummy 
argument of type character.  A character constant is stored as a consecutive 
string of characters in memory, and its starting location is passed as the 
argument address.  Thus, in the example, 
    CALL FOR ('PHILIP MORRIS')    SUBROUTINE FOR(M) 
    .                             CHARACTER*4,M(5) 
    . 
                                        or 
                                  SUBROUTINE FOR(C) 
                                  CHARACTER*1,C(13) 
the following correspondences hold: 
    M(1) = 'PHIL'     C(1) through C(13) are defined to be 'P', 
                      'H', etc. 
    M(2) = 'IPbM' 
    M(3) = 'ORRI' 
    M(4) = 'Sbbb' 
    M(5) = undefined and not to be referenced. 
Actual arguments that are literal constants may be received by dummy arrays. 
(They may also be passed to assembly language subprograms.)  A literal 
constant is stored as a consecutive string of characters in memory, and its 
starting location is passed as the argument address.  Thus, in 
    CALL FOR ('PHILIP MORRIS')     SUBROUTINE FOR(M) 
    . 
    .                              INTEGER M(5) 
    . 
or 
    CALL FOR (13HPHILIP MORRIS) 
the following correspondences hold: 
    M(1) = 4HPHIL 
    M(2) = 4HIPbM 
    M(3) = 4HORRI 
    M 4) = 4HSbbb 
    M(5) = undefined and not to be referenced. 
Literal constants are filled out with trailing blanks (b represents the blank 
character) to the nearest word boundary (multiple of four characters). 
Therefore, passing such a constant to a dummy of a type that occupies more 
than one word per element (e.g., double precision) may result in dummy 
elements that are only partially defined.  (See VARIABLE_ALLOCATION.) 
Furthermore, literal constants do not necessarily begin on doubleword 
boundaries, so they may not be accessed correctly by doubleword instructions. 
For these reasons, integer arrays are the recommended numeric data for 
receiving literal constant arguments. 
All literal constant arguments are preceded by a word containing a count of 
the number of characters in the string.  This is primarily for use in assembly 
language subprograms.  The count may also be accessed by FORTRAN subprograms, 
although this requires the use of a subscript that is out of range (i.e., one 
less than the lower bound of the array).  Thus, in the above example, M(O) 
would contain the count, and might be used as follows: 
    SUBROUTINE FOR (M) 
    INTEGER M(5),MN(20) 
    KOUNT=M(0) 
    DECODE(20,1,M)(MM(I),I=1,KOUNT) 
 1  FORMAT(20A1) 
If an array corresponds to something that is not an array or a literal 
constant, the latter will correspond to the first element of the array.  This 
is true whether the calling argument is an array and the dummy is not, or vice 
versa.  Thus, for example, if the calling argument is a variable, and the 
dummy is an array, references in the subprogram to elements of the array other 
than the first element will correspond to whatever happens to be stored near 
the variable.  Care must be taken in creating correspondences of this nature, 
as they may depend on a particular implementation. 
Since a dummy array does not actually occupy any storage, its dimensions are 
used only to locate its elements, not to allocate storage for them.  Therefore 
the dimensions of a dummy array do not have to be defined within the 
subprogram in the normal manner.  Instead, any or all the dimensions of a 
dummy array may be specified by variables rather than constants.  (Each 
variable must be either in common storage or global storage, or be a dummy 
argument.)  This permits the calling program to supply the dimensions of the 
dummy array each time the subprogram is called.  The following statements 
demonstrate adjustable dimensions. 
DIMENSION P(10,5),   FUNCTION SUM(R,N,M)   FUNCTION QSUM(Z)   FUNCTION SUB1(A) 
   Q(9,3)U(10,10,5)  DIMENSION R(N,M)      COMMON /A/I,J      REAL A(10,10,*) 
COMMON /A/I,J         .                    REAL Z(I,J)         . 
I=9                   .                     . 
J=3                   .                     . 
X=SUM(P,10,5)                               . 
Y=SUM(Q,9,3) 
Z=QSUM(Q) 
Z1=SUB1(U) 
  . 
  . 
  . 
  . 
Only a dummy array may be given adjustable dimensions, and the dimensions must 
be specified by integer expressions.  The operators in an adjustable dimension 
integer expression may be either +, -, *, /,or **.  The operands in an 
adjustable dimension expression are limited to integer constants and integer 
variables.  Note that function references and array elements are not allowed. 
These variables are normally dummies also, but they need not be.  They may, 
for example, be GLOBAL.  If not dummies, they must be set prior to entry into 
the subprogram.  Both upper and lower bounds of a dimension may be adjustable, 
as in 
    DIMENSION DUMMY(I:J,O:L,M,100) 
    DIMENSION DUMMY(I:5,O:L,M,*) 
The variables used as adjustable dimensions may be referenced elsewhere in the 
subprogram.  However, the appearance of any of these variables in the dummy 
list of a succeeding ENTRY statement constitutes a change in the declaration 
of the array.  Therefore, an ENTRY statement that contains any of the 
dimensions of an adjustable array must also contain the array name.  The 
appearance of the array name in the ENTRY statement requires the appearance of 
all the dimensions that are dummies and causes the array bounds to be 
recomputed.  See ENTRY. 
A dummy array that is declared as an assumed size array (an '*' is used as the 
upper bound in the last dimension of the array) may not be used unsubscripted 
in an I/O list. 
A dummy subprogram must correspond to an argument that is a subprogram name, 
and it is the only kind of dummy that may do so.  The dummy name merely serves 
to identify a closed subprogram whose actual location is defined by the 
calling program.  Therefore, a call on a dummy subprogram is actually a call 
on the subprogram whose name is specified as the actual argument.  A dummy 
subprogram is classified in the same manner as any other subprogram (see 
CLASS_OF_IDENTIFIERS). 
Examples: 
    INTRINSIC SIN, DSIN,SQRT,DSQRT     FUNCTION DIFF(F,DF,Z) 
    A=DIFF(SIN,DSIN,X)                 DOUBLE PRECISION DF 
    B=DIFF(SQRT,DSQRT,Y)               DIFF=DABS(F(Z) - DF(Z)) 
    .                                  RETURN 
    . 
    .                                  END 
If an identifier to be passed as an argument cannot otherwise be ascertained 
to be a subprogram identifier, it must appear in an EXTERNAL statement (or an 
INTRINSIC statement if the intrinsic function is desired) to so define it. 
Otherwise, it will be classified as a variable.  In general, it is a good 
practice to declare as EXTERNAL all nonintrinsic subprogram names to be used 
as arguments, and to declare as INTRINSIC all intrinsic function names to be 
used as arguments. 
00001
A dummy variable corresponds to a single item in the calling program. 
A dummy that is not declared (implicitly or explicitly) to be an array or a 
subprogram is treated as a variable. 
Syntax: 
CALL {DUMP|PDUMP} (a1,b1,f1,a2,b2,f2,...,an,bn,fn) 
Parameters: 
ai and bi    are variables, array names, or array elements that indicate the 
upper and lower limits of storage to be dumped.  Either ai or bi may represent 
the upper or lower limits. 
fi    is an integer expression whose value indicates the dump format desired. 
  fi  Data Interpreted As      Per Line   Format Used 
0   Hexadecimal words           8      Z12 
1~  Byte logical               32      L2 
    (same as 2) 
2   Logical                    32      L2 
3~  Half-word integer           8      I12 
    (same as 4) 
4   Integer                     8      I12 
5   Real                        4      1PG24.7 
6   Double precision            4      1PG24.16 
7   Complex                     2      (4X,1H(1PG14.7,1H,1PG14.7,1H)) 
8   Double precision complex    2      (4X,1H(1PG22.16,1H,1PG22.16,1H)) 
9   Literal words              16      A6 
10  Octal words                 8      015 
---------------- 
~ These values are accepted to provide compatibility with systems that 
use byte logical and half-word integer data. 
1.  The DUMP and PDUMP subprograms are identical except that after a DUMP 
    reference, execution of the program is terminated (i.e., a STOP statement 
    is simulated), while after PDUMP (P meaning Proceed) control returns to 
    the user's program. 
2.  The last format code, Fn, may be omitted.  It is then assumed to be zero 
    and the dump will be in hexadecimal format.  Example: 
        CALL PDUMP(A,A(10)) 
3.  The variables ai and bi should both be in the same storage allocation area 
    (i.e., both in blank COMMON, the same labeled COMMON block, or the same 
    program unit), since there is no guarantee as to what lies between these 
    areas. 
4.  Regardless of whether the upper or lower limit is specified first, the 
    area is always dumped from lower to upper; it is never dumped backward. 
    The lower limit is normally specified first. 
5.  The dump information is written on the standard Listing Output unit, which 
    is logical unit number 108.  This number is normally assigned to the LO op 
    label (which is normally assigned to the line printer), but can be 
    reassigned by the user if desired. 
6.  Each line of output is preceded by a six-digit octal address preceded by 
    the segment number of the first item on the line; e.g., 
        02 000567 
        02 002456 
7.  The interpretation of the format codes (fi) and the formats used for 
    output are shown.  Note that Z represents hexadecimal format, which is 
    similar to R format but operates on four bits at a time rather than nine. 
    Octal format is also similar to R format but operates on three bits at a 
    time instead of nine. 
8.  Each new range of storage (ai through bi) begins on a new line.  / 
9.  Whereas the OUTPUT statement uses mostly widthless formats, the DUMP 
    subprograms use constant width so that values of the same type will line 
    up in columns. 
00001
OVERFL_DVCHK
PRINT_POSITIONING
Syntax: 
ELSE 
Description: 
An ELSE-block consists of all of the executable statements that appear 
following the ELSE statement up to, but not including, the next END IF 
statement that has the same IF-level as the ELSE statement.  An ELSE-block may 
be empty. 
Execution of an ELSE statement has no effect. 
Transfer of control into an ELSE-block from outside the ELSE-block is 
prohibited.  The statement label, if any, of an ELSE statement must not be 
referenced by any statement. 
00001
IF_ELSE
ELSE
Embedded comments may be used in FORTRAN-77.  Comments may be added at the end 
of FORTRAN source lines.  The exclamation mark character (!) signals the end 
of the FORTRAN source code and the beginning of comments. 
Example: 
DO WHILE  A .NE. B    ! first DO WHILE loop 
  A=A+B               ! will loop 0 to 1 time 
ENDDO 
DO WHILE , X .EQ. Y   ! second DO WHILE loop 
  X=X+Y               ! will loop forever 
ENDDO 
DO WHILE , (A .NE. X) ! third DO WHILE loop 
  A=X+1               ! depends on initial value of A and X 
ENDDO 
END 
See also:  COMMENTS 
00001
Syntax: 
ENCODE (c,f,s[,n])[,][k] 
Parameters: 
c    defines the number of characters per internal record.  It is an 
expression of integer, real, or double precision type, truncated to integer if 
necessary. 
f    specifies a FORMAT statement.  It may be the statement label of a FORMAT 
statement, a variable that has had such a statement label ASSIGNed to it, or 
the name of an array in which a FORMAT has been stored. 
s    indicates the first element or starting location of the internal buffer. 
It may be an array name, an array element, or a variable name.  Any data type 
is acceptable; character is recommended. 
n    is an optional integer variable into which, upon completion of the 
operation, the number of characters actually processed (generated or scanned) 
will be stored. 
k    is an input/output list. 
Description: 
The ENCODE statement causes the list items to be converted to character 
strings, according to the FORMAT specified by f, and to be placed in storage 
beginning at location s. 
If the number of characters generated by the FORMAT statement is greater than 
the specified size of the record, the extra characters are lost; they are not 
filled into the following record.  If fewer characters are generated than are 
necessary to fill the record, it is filled out with trailing blanks.  In fact, 
on ENCODE operations as on formatted WRITE operations, the first thing done 
with each record is to fill it with blanks; this is done before any characters 
are stored (generated) into it. 
Examples: 
The following statements might be used to create a FORMAT stored, for later 
use, in the array M: 
    ENCODE(12,3,M)12,5 
    3 FORMAT(2H(F,I,1H.,I,1H)) 
The FORMAT so created would occupy the first three elements of M and would 
appear as: 
    (F12b.5b)bbb 
00002
00003
00001
The ENCODE and DECODE statements are similar to formatted WRITE and READ 
statements.  In an ENCODE/DECODE operation, however, instead of actual 
input/output, data conversion takes place between an input/output list and an 
internal buffer area.  This buffer area is specified by the programmer and is 
usually an integer array.  Whereas an external record has a certain physical 
length, the length of the simulated internal record in an ENCODE/DECODE 
operation may be specified by the programmer. 
ENCODE
Syntax: 
END 
Description: 
An END statement is used to inform the FORTRAN-77 compiler that it has reached 
the physical end of a program.  If, in a main program, control reaches an END 
statement, the effect is that of a STOP statement.  An END statement may be 
labeled.  An END statement in a subroutine acts as a RETURN. 
The following restriction applies to any statement that begins with the 
character string E N D: 
    If, at the end of any FORTRAN initial line, the compiler has encountered 
    only the characters E N D, it assumes that the statement is an END 
    statement and will act accordingly. 
This limitation is due to an historic FORTRAN feature (and the ANSI FORTRAN 
standard) -- namely, the way in which continuation is specified.  As indicated 
by the left-hand example below, certain statements (although legitimate 
FORTRAN statements) will be processed as though they were END statements. 
Examples: 
    Processed as END Statements          Not Processed as END Statements 
    column:  6  7..............          6  7................. 
                END                         END FILE 
             1  FILE2                    1  2 
                END                         END RA 
             X           RATE=A*B        X             TE=A*B 
                END                         END(I,J 
             X           (I,J,,) 
                         =.NOT. Q        X  ,K)=.NOT. Q 
             0  END                      0  E 
                                         1  N 
                                         2  D 
Similar statements of the same nature as those in the first column will be 
treated as END statements. 
00001
00002
Syntax: 
END DO 
Description: 
The END DO statement terminates the range of a DO or DO WHILE statement.  The 
END DO statement must be used to terminate a DO block, if the DO or DO WHILE 
statement defining the block does not contain a terminal statement label. 
Example: 
    DO WHILE (I .GT. J) 
        ARRAY(I,J) = 1.0 
        I = I - 1 
    END DO 
00001
00002
ENDDO
Syntax: 
ENDFILE {e|(u)|(u,ERR=s,IOSTAT=ios)} 
Parameters: 
e    is an integer, real, or double precision expression whose value 
(truncated to integer if necessary) determines the unit on which the endfile 
record is to be written. 
u    is an integer, real, or double precision expression, or the positional 
form of unit specifier, UNIT=e. 
s    is a statement label. 
ios    is an integer variable or array ID, that is set to an error code (see 
VALUES_OF_IOS) if an error occurred during the ENDFILE operation.  It will be 
set to zero otherwise. 
Description: 
This statement causes an endfile record to be written on the specified unit, 
creating an endfile record as the next record of the file.  This endfile 
record becomes the preceding record in position as well as the last record of 
the file. 
Sometimes it is desirable to take a program that has been written for output 
on magnetic tape and assign that logical unit number to some other device 
(such as a line printer).  Since such programs often write an end-of-file and 
rewind each of their tapes at the end of the job, it is permissible to specify 
an ENDFILE or REWIND operation on any device.  When the device is not a 
magnetic tape or sequential disk file, the statements have no effect.  It is 
not permissible to BACKSPACE such devices. 
00002
00001
ENDFILE
ENDDO
READ_END_ERR_IOSTAT
WRITE_END_ERR_IOSTAT
Syntax: 
END IF 
Description: 
Execution of an END IF statement has no effect. 
For each block IF statement there must be a corresponding END IF statement in 
the same program unit.  A corresponding END IF statement is the next END IF 
statement that has the same IF-level as the block IF statement. 
Examples: 
    IF (X>5.) THEN 
       OUTPUT,X,'X IS TOO LARGE' 
       X=1.0 
    ELSE IF (A>C) THEN 
       OUTPUT,'A IS TOO LARGE' 
    ELSE 
       OUTPUT,X,'X IS OK' 
    END IF 
    IF (TYPE .EQ. HAT) THEN 
    ELSE IF (SIZE .EQ. 7) THEN 
       STRING='SMALL' 
    ELSE IF (SIZE .EQ. 8) THEN 
       STRING='LARGE' 
    ELSE IF (SIZE .EQ. 9) THEN 
       STRING='GIGANTIC' 
    ELSE 
       STRING='NONE' 
    END IF 
00001
00002
END_IF
Unless otherwise specified, execution of the FORTRAN program is terminated 
when an end-of-file mark is encountered during an input operation.  There are 
two ways to override this feature: 
1.  Each individual READ statement or INPUT statement may specify the action 
    to be taken if an end-of-file is encountered during the processing of that 
    READ statement.  (See READ_END_ERR_IOSTAT.) 
2.  The library subroutine EOFSET may be used to set up a universal 
    end-of-file exit location, which will be in effect on all input operations 
    except those that specify their own end-of-file exit, as in paragraph 1 
    above, or BUFFERIN, or BUFFIN operations.  The name SETEOF may also be 
    used, but with the argument specified in the opposite order. 
END
Syntax: 
ENTRY p[(d1,d2,d3,...dn)|()] 
Parameters: 
p    is the name of the entry point. 
di    is a dummy argument of any of the forms discussed under 
ARGUMENTS_DUMMIES. 
Description: 
When control is transferred to a subprogram through an ENTRY statement, 
execution begins at the first executable statement following the ENTRY 
statement.  The ENTRY statement itself is nonexecutable and does not affect 
the flow of the program in which it appears.  That is, program flow can pass 
through an ENTRY statement. 
Examples: 
    SUBROUTINE FINISH(N) 
    GLOBAL FLAG(7) 
    END FILE N 
    ENTRY REWIND (N) 
    REWIND N 
    FLAG (N)=0 
    RETURN 
    END 
The dummy arguments in an ENTRY statement need not agree with those in the 
FUNCTION or SUBROUTINE statement, nor with those in other ENTRY statements. 
However, they may agree if desired, as in the above example.  When the same 
dummy name appears in an ENTRY statement and in the FUNCTION or SUBROUTINE 
statement (or another ENTRY statement), it does not refer to two separate 
entities; it represents the same quantity and, as such, must agree in class 
and type between the two entry points (e.g., one cannot be a scalar, the other 
an array; or one real and the other integer). 
When a dummy name appears at more than one entry point, it need not appear in 
the same position in the dummy list; that is, dummy names do not correspond to 
argument list positions.  When a subprogram is entered, all of the dummies in 
the SUBROUTINE, FUNCTION, or ENTRY statement are set up to correspond to the 
arguments in the call on that statement, overriding any previous 
correspondence that may have existed.  Thus, in the example shown above, if 
the calls 
    CALL FINISH(3) 
    CALL REWIND(5) 
are made, the statement REWIND N is interpreted as REWIND 3 the first time and 
as REWIND 5 the second. 
Dummy correspondences set up by any call on a SUBROUTINE, FUNCTION, or ENTRY 
statement do not remain in effect during all subsequent calls on any entry 
point in the subprogram.  Thus, it is not permissible to "initialize" a dummy 
with one call and make use of this initialization on subsequent calls, and 
references to a dummy correspondence from a prior call or a different entry 
point are not permitted. 
Rules: 
The following rules also apply to ENTRY statements: 
1.  The result of a FUNCTION is returned in the FUNCTION name or in the ENTRY 
    name.  The ENTRY name serves to identify the location of the entry point 
    and identify the value to be returned by the function when the entry point 
    is entered. 
2.  ENTRY statements do not alter the rules concerning placement of statement 
    functions.  Statement functions may appear after an ENTRY statement only 
    if they will appear before the first executable statement in the 
    subprogram. 
3.  No subprogram may refer to itself, either directly or indirectly through 
    any of its entry points, nor may it refer to any subprogram whose RETURN 
    statement has not been executed. 
4.  An ENTRY statement may not appear in a main program. 
5.  Before being referenced in any executable statement every dummy should 
    have appeared in a SUBROUTINE, FUNCTION, or ENTRY statement.  In the 
    following example, the use of A, Z and B is correct; that of X and Y is 
    not: 
        SUBROUTINE ALPHA(A) 
        A=X 
        INTEGER B,Y(10),Z(10) 
        A=Y(3) 
        ENTRY BETA (A,B,X,Y,Z) 
           . 
           . 
           . 
6.  If any of the dimensions of an adjustable dummy array appear in the dummy 
    list of an ENTRY statement, then the array name must also appear there, 
    and vice versa.  (See DUMMY_ARRAY_DIMENSIONS.) 
7.  If an entry name in a function subprogram is of type character, each entry 
    name and the name of the function subprogram must be of type character. 
    If the name of the function subprogram or any entry in the subprogram has 
    a length of (*) declared, all such entities must have a length of (*) 
    declared; otherwise, all such entities must have a length specification of 
    the same integer value. 
00002
00003
00001
00004
Syntax: 
CALL EOFSET[(k[,n])] 
Parameters: 
k    is a statement label argument (i.e., a statement label followed by the 
letter S, or preceded by an asterisk (*) or an ampersand (&), or is a variable 
to which the location of a statement has been assigned (with an ASSIGN 
statement). 
n    is an integer variable, into which the logical unit number of the device 
on which the end-of-file occurred will be stored. 
Description: 
After EOFSET has been called with a statement label argument (k), an 
end-of-file causes an immediate transfer to the statement referenced by k.  If 
n is specified, it is assigned the value of the logical unit number.  Often 
this variable is used in a computed GO TO statement to determine what action 
is to be taken. 
Examples: 
     CALL EOFSET (*332, LUNIT) 
     . 
     . 
     . 
 322 GO TO(3,8,49,5,6,102),LUNIT 
     . 
     . 
In the above example, when an end-of-file is encountered, LUNIT is assigned 
the value of the logical unit number and a transfer occurs to the statement 
with the label 332. 
The transfer location may be reset any number of times by successive calls to 
EOFSET.  When EOFSET is called with no arguments, the subsequent occurrence of 
an end-of-file causes termination of execution. 
00002
00003
00001
Syntax: 
EQUIVALENCE s1,s2,s3,...,sn 
Parameters: 
each si    is an equivalence set of the form 
    (v1,v2,v3,...,vm) 
Each equivalence set specifies that all of the vi are to be assigned the same 
storage location.  The vi may have one of the following three forms: 
1.  A variable or an array name.  For arrays, the location referred to is that 
    of the first element. 
2.  An array element, where each subscript is a constant integer expression. 
    For example, the statements 
          REAL B,C,A(3,3),X(-5:5) 
          EQUIVALENCE (A(1,3),B),(C,X(1)) 
    make B and A(1,3) equivalent, and, similarly, C and X(1) equivalent. 
3.  A variable or array name followed by a constant integer expression, 
    denoting the element count, enclosed in parentheses.  The meaning of this 
    count is as follows:  the location of the variable, or the first element 
    of the array, is denoted as position 1.  The element immediately following 
    is position 2, and so on.  The element immediately preceding is position 
    0, the element preceding that is position -1, and so on.  Thus if X is a 
    scalar, X(1) means the same as X; X(3) is two elements beyond X, where the 
    size (in words) of an element is dependent of the type of X.  The same is 
    true if X is an array of other than one dimension. 
Note, however, that if X is a one-dimensional array, this form does not apply, 
since X(1) is an array element, as described in paragraph 2.  Normally, this 
case, X(1) is still the same as X, i.e., it is the first element of X.  If, 
however, the lower dimension bound of X is not 1, this is not true; X(1) will 
not be the first element. 
The following statements accomplish the same equivalence as those in paragraph 
2: 
    REAL B,C,A(3,3),X(-5:5) 
    EQUIVALENCE (A(7),B(1)),(C(-5),X) 
Description: 
The EQUIVALENCE statement controls the allocation of variables relative to one 
another.  Generally it is used to assign more than one variable to the same 
storage location or locations. 
In an equivalence set an item of type character may only be equivalenced to 
another item of type character.  Thus, if v1 represents a datum of type 
character, v2...vm must all be of type character or byte logical; if v1 does 
not represent a datum of type character, v2...vm may not represent an item of 
type character.  Character substring names are allowed in equivalence 
statements.  The substrings must be constant expressions. 
A dummy (subroutine argument) may not be equivalenced to any other item. 
An EQUIVALENCE reference to a variable of type occupying other than one word 
per element (e.g., COMPLEX) is interpreted as referring to the beginning of 
the first element (i.e., to the first byte).  See also 
STORAGE_ALLOCATION_RULES for further rules concerning equivalences that cannot 
be implemented. 
Examples: 
The statements 
    DIMENSION W(3),X(0:2,0:2),LC(7) 
    REAL W,X 
    INTEGER LC,J 
    DOUBLE PRECISION ELSIE 
    COMPLEX C 
    EQUIVALENCE (W,LC(2),ELSIE),(X(6),J,C(3)) 
cause the following equivalences: 
    Word        Variables - Set 1          Variable - Set 2 
    ----        -----------------          ---------------- 
       1        LC(1)                      X(0,0) 
       2        LC(2) = W(1) = ELSIE1      X(1,0) = C1 
       3        LC(3) = W(2) = ELSIE2      X(2,0) = C2 
       4        LC(4) = W(3)               X(0,1) 
       5        LC(5)                      X(1,1) 
       6        LC(6)                      X(2,1) = J 
       7        LC(7)                      X(0,2) 
       8                                   X(1,2) 
       9                                   X(2,2) 
where the arrangement of set 1 has no bearing on the arrangement of set 2. 
Note that the statement 
    EQUIVALENCE (LC(2),W),(W(1),ELSIE),(C(3),J),(J,X(6)) 
has the same results as the EQUIVALENCE statement in the previous example, and 
that the set (J,X(2,1)) is the same as the set (J,X(6)) in this case. 
00002
00003
00001
Syntax: 
CALL ERROROUT(u,ios) 
Parameters: 
u    is an integer expression that specifies the logical unit number of the 
device to which the error message will be written. 
ios    the integer number that was assigned to the IOSTAT parameter when an 
error code is returned to the user after an I/O operation has been attempted. 
Description: 
Decodes the IOSTAT code into the proper error message, and prints the error 
message. 
00002
00001
Some additional conventions are necessary. 
1.  At any one subexpression level of evaluation, operations of the same order 
    of precedence (except for exponentiation) are evaluated from left to 
    right.  Consequently, I/J/K*L is equivalent to ((I/J)/K)*L.  However, note 
    that when the result is algebraically the same, the compiler may reorder 
    operations having the same precedence to improve efficiency.  For example, 
    A+B+C*D may be evaluated as C*D+B+A. 
2.  Consecutive exponentiations are performed right to left, thus 
        A**B**C 
    is interpreted as 
        A**(B**C) 
    The use of parentheses is recommended, as many FORTRAN systems interpret 
    consecutive exponentiation differently. 
3.  The sequence "operator operator" is not permissible.  Therefore, A*-B must 
    be expressed as A*(-B). 
4.  As in algebraic notation, parentheses are used to define evaluation 
    sequences explicitly.  Thus, A plus B divided by C is written as (A+B)/C. 
5.  In the expression 
        - A ** 2 
    The exponentiation operator (**) has precedence over the negation operator 
    (-); therefore, the operands of the exponentiation operator are combined 
    to form an expression that is used as the operand of the negation 
    operator.  The interpretation of the above expression is the same as the 
    interpretation of the expression 
        - (A ** 2) 
    Unsigned constants are primaries for exponential operations, as opposed to 
    signed constants.  Therefore: 
        -2**N 
    is actually treated as being 
        -(2**N) 
Examples: 
    The expression 
        A*(B+C*(D-E/(F+G)-H)+P(3)) 
    is evaluated in the following sequence: 
        r1=F+G 
        r2=E/r1 
        r3=D-r2-H 
        r4=C*r3 
        r5=B+r4+P(3) 
        r6=A*r5 
where the ri are the intermediate results of the various levels of evaluation. 
00001
The expression 
    A+B/C 
might be evaluated as 
    (A+B)/C 
or as 
    A+(B/C) 
The latter form represents FORTRAN's evaluation of the first form.  However, 
it is necessary to formulate rules for expression evaluation so that such 
ambiguities do not occur. 
The complete set of program units that are executed together as a single job 
is called an executable program.  An executable program consists of one main 
program and all required subprograms.  Subprograms may be: 
o   defined by the programmer (as described below) 
o   may be provided by the processor (compiled in line by the compiler) 
o   may be contained in the system libraries (preprogrammed) 
o   may be provided by both the processor in line and the system libraries 
There are two kinds of subprograms, executable and nonexecutable. 
Nonexecutable subprograms are used to initialize data in labeled common and 
global data regions.  BLOCK DATA subprograms are nonexecutable subprograms. 
Executable subprograms contain procedure (as a result of compiling executable 
statements) and data, possibly with initial values for data items. 
Main programs are executable and contain procedure and data similar to 
executable subprograms. 
Syntax: 
EXIT 
Description: 
The EXIT statement allows the user to exit the current DO loop being executed. 
The EXIT statement, when executed, causes the current DO loop to be exited 
with control going to the first instruction immediately following the current 
DO loop.  This statement gives the user the ability to exit a DO loop prior to 
the terminal statement of the DO loop without having to use a GO TO statement. 
Example: 
    DO 
        IF(A .GT. B) EXIT 
        A = A + 1 
    END DO 
00001
00002
EXIT
Syntax: 
CALL EXIT [(number)] 
Parameters: 
number    is an integer expression whose value is between 0 and 511. 
Description: 
*EXIT* is printed on the listing output device and the job is terminated.  If 
the argument is passed, STEPCC is set to 'number' and the *EXIT* or *STOP* is 
not printed. 
00002
00001
All declarations other than implicit are explicit declarations.  Explicit 
declarations are required to classify an identifier in any way other than 
described above.  Explicit declarations include 
    constant declarations 
    array declarations 
    type declarations 
    storage allocation declarations 
    subprogram declarations 
    subprogram definitions 
Explicit declarations override implicit declarations and should precede the 
first use (in an executable statement or a DATA statement). 
Except where specifically noted to the contrary, definitions and declarations 
of the classification of an identifier may not conflict.  For example, an 
identifier may not be both a subprogram name and an array name, be both 
integer and real type, be defined as a subprogram in more than one place, etc. 
Redundant declarations are not permissible. 
00001
00001
00001
00001
Syntax: 
type s1,s2,s3,...,sn 
Parameters: 
type    is one of the following declarations:  (See also OPTIONAL_SIZE_SPECS). 
    INTEGER*2 
    INTEGER 
    REAL 
    DOUBLE PRECISION 
    COMPLEX 
    DOUBLE COMPLEX 
    LOGICAL 
    LOGICAL*1 
    CHARACTER or CHARACTER*Z 
    where 
    Z    specifies the length in characters and is an integer constant, or a 
    constant integer expression enclosed in parentheses, or an asterisk 
    enclosed in parentheses.  The CHARACTER*(*) form allows the user to pass 
    the size of the character identifier from the calling routine.  The dummy 
    character identifier uses the length of the actual argument.  If Z is not 
    specified, 1 is assumed. 
si    is a type specification that is either the identifier of a variable, 
array, or function, or is an array declarator.  Optionally, a variable, array, 
or array declaration may be followed by a DATA constant list enclosed in 
slashes, for the purpose of defining initial values.  In other words, each 
type specification may take any of the following forms: 
    identifier 
    array declarator 
    identifier/DATA constant list/ 
    array declarator/DATA constant list/ 
Additional forms allowable in a CHARACTER statement include: 
    identifier*Z 
    array declarator*Z 
    identifier*Z/DATA constant list/ 
    array declarator*Z/DATA constant list/ 
For a description of DATA constant lists and their function, see 
DATA_STATEMENT. 
Note that: 
    REAL X,Y,Z/3.7/ 
initializes Z only, whereas 
    DATA X,Y,Z/3.7/ 
is in error. 
Description: 
Explicit type statements are used to define, explicitly, the type of an 
identifier.  An explicit type declaration overrides any implicit declaration. 
Thus, the statements 
    IMPLICIT LOGICAL(L-P) 
    REAL LEVEL,PERICOSITY 
in combination with the standard implicit typing rule, causes the following 
identifiers to have the types indicated: 
    Identifier     Type 
    ----------     ---- 
    LEVEL3         Logical 
    LEVEL          Real 
    KAPPA          Integer 
    POROUS         Logical 
    PERICOSITY     Real 
    X              Real 
Type statements must precede the first use of the identifiers specified. 
Examples: 
    COMPLEX C3,ALPHA,CARRAY(5,5),XYZ 
    LOGICAL BINARY,BOOLE(4,4,4,4),TRUTHF 
    REAL GEORGE,NETRAT(0:8)/0,1,1,2,3,5,8,13,21/,MASS/0/ 
    DOUBLE COMPLEX DCROOT,PP 
    CHARACTER*5A,B*3,C(5,5),D(5,5)*7,E/'ABCDE'/ 
    CHARACTER*(*) 
00002
00003
00001
Expressions are strings of operands separated by operators.  Operands may be 
constants, variables, array elements, or function references; an expression 
may contain subexpressions.  Subexpressions are expressions enclosed in 
parentheses.  Operators may be unary, that is they may operate on a single 
operand, or they may be binary, operating on pairs of operands.  Expressions 
may be classed as arithmetic, relational, logical, or character.  All 
expressions are single valued; the evaluation of any expression has a unique 
result. 
The following FORTRAN-77 expressions are discussed: 
    ARITHMETIC_EXPRESSIONS 
    CHARACTER_EXPRESSIONS 
    RELATIONAL_EXPRESSIONS 
    RELATIONAL_OPERATORS 
    LOGICAL_EXPRESSIONS 
    LOGICAL_OPERATORS 
To display a particular expression (such as Logical), enter: 
HELP (FORTRAN) LOGICAL_EXPRESSIONS 
LOGICAL_EXPRESSIONS
ARITHMETIC_EXPRESSIONS
CHARACTER_EXPRESSIONS
MIXED_EXPRESSIONS
RELATIONAL_EXPRESSIONS
Syntax: 
v1=v2=v3=...=vn=e 
Parameters: 
each vi    is either a variable or an array element. 
e    is an expression, as described for the assignment statement. 
Description: 
An extended assignment statement can be used to assign a value to more than 
one variable and array element.  Assignments of this kind are performed from 
right to left.  That is, the value of e is assigned to vn, the value of vn is 
assigned to vn-1, and so on.  Across each individual assignment, type 
conversion is performed, if necessary, according to the MIXED_ASSIGNMENT_TYPES 
table. 
Examples: 
    C=I=R=3*D+14.5 
is equivalent to the statements 
    R=3*D+14.5 
    I=R 
    C=I 
in that order.  If C is complex, I is integer, R is real, D is double 
precision, and the value of the expression 3*D+14.5. is 349.625038426147, then 
the following assignments would occur: 
    R=349.625 
    I=349 
    C=(349.0,0.0) 
Other examples of extended assignment statements are 
    A=B=C=D=J=N=O 
    L1=L2=L3= .TRUE. 
    VAL(K,3)=VAL(K+1,3)=VAL(K-1,3)*RATIO3+FUDGE FACTOR 
    TIME(LIME)=ROARING(20,20)=GOODOLD*GONEBY 
    INITIAL VALUE=FINAL VALUE=INPUT VALUE 
Also, 
    KROQ=WOOF(KROQ)=KROQ=IHS+1020 
is equivalent to the statements 
    KROQ=IHS+1020 
    WOOF(KROQ)=KROQ 
    KROQ=WOOF(KROQ) 
in that order. 
00002
00003
00001
Syntax: 
EXTERNAL p1,p2,p3,...,pn 
Parameters: 
pi     are the subprogram identifiers. 
Description: 
This statement declares that the identifiers listed are subprogram identifiers 
that may be referenced as external "closed" routines.  It serves as a 
declaration of external reference, as follows: 
1.  The subprogram is not defined within this program unit.  This statement 
    serves merely to declare that it is a subprogram and is defined in some 
    other program unit. 
2.  If the subprogram is a dummy, its appearance in an EXTERNAL statement 
    serves merely to declare that it is a subprogram and corresponds to a 
    (closed) subprogram in the calling program.  This declaration is neither 
    one of external definition nor one of external reference. 
The practical use of the EXTERNAL statement, then, is to declare as 
subprograms names that might otherwise be classified implicitly as variables, 
so that they may be passed as arguments to other subprograms.  (See 
ARGUMENTS_DUMMIES.) 
Examples: 
If the subprogram name F appears in the statement 
    CALL ALPHA(F) 
but appears in no other context to indicate that it is a subprogram, it will 
be implicitly classified as a variable.  The EXTERNAL statement can be used to 
avoid this.  For compatibility with other FORTRAN systems, it is recommended 
that all subprogram names that are passed as arguments appear in EXTERNAL 
statements.  Note that F may itself be a dummy subprogram. 
Usage Notes: 
An EXTERNAL declaration does not specify the type of an identifier.  The 
EXTERNAL statement removes any processor predefinition of the subprogram pi. 
In particular, the appearance of an intrinsic function name declares that name 
no longer is an intrinsic function in the program unit.  The type of that 
function name is not determined by the implicit typing rules. 
00002
00003
00001
00004
CONCEPTS
A file is a set of zero, one, or more records.  A record is a member of 
exactly one file.  There are four kinds of files: 
SEQUENTIAL 
DIRECT_ACCESS 
INTERNAL 
KEYED 
Sequential files, direct-access files and keyed files are collectively 
referred to as external files. 
The following file types and related subjects can be displayed: 
FILE_EXISTENCE 
FILE_NAMES 
SEQUENTIAL 
DIRECT_ACCESS 
KEYED_ACCESS 
INTERNAL 
To display a particular file (such as Sequential), enter: 
HELP (FORTRAN) FILES SEQUENTIAL 
To receive a display of all file types and subjects, enter ??. 
FILE_EXISTENCE 
Any file that is connected to a logical unit or capable of being connected to 
a logical unit for an input operation is said to exist.  A file that cannot be 
connected for an input operation is said not to exist.  Note that a file may 
exist and have no records. 
To create a file means to bring into existence a file that did not previously 
exist.  To delete a file means to terminate the existence of the file. 
Certain input/output statements, such as READ and WRITE, may refer only to 
files that exist.  Other input/output statements, such as INQUIRE, OPEN, and 
CLOSE, may refer to files that do not exist. 
FILE_NAMES 
A file may have a name.  The name of the named file is a character string. The 
class of allowable names is system-dependent. 
SEQUENTIAL 
A sequential file has the following properties: 
1.  If the file contains one or more records, these records exist as a totally 
    ordered set.  There is a first record and a last record.  The order of the 
    records is the order in which they were written. 
2.  The file may contain either formatted, unformatted, free-form, 
    self-identified, or any record combination thereof.  The last record in 
    the file may be an endfile record.  Records may be of different lengths in 
    the same file. 
3.  The file has a position attribute, from which are derived the related 
    concepts of next record, preceding record, current record, and initial 
    point.  Certain circumstances (usually error conditions) can cause the 
    position of the file to become undefined. 
4.  If the file is positioned between the ith record and the ith+1 record, the 
    ith record is the preceding record and the ith+1 is the next record, and 
    there is no current record. 
5.  The initial point of the file is that unique point where the next record 
    is the first record of the file.  When the file is positioned at its 
    initial point, there is no preceding record in the file and no current 
    record. 
6.  When the file is positioned after the last record, there is no current 
    record and no next record in the file.  The last record is the preceding 
    record. 
7.  If the file contains no records, there is no preceding record, no current 
    record, and no next record. 
8.  Transfer of records to or from a sequential file is not permitted in 
    direct access input/output statements. 
DIRECT_ACCESS 
A direct-access file has the following properties: 
1.  The file contains either formatted records or unformatted records or both. 
    It does not contain self-identified records or endfile records. 
2.  The length of every record in the file is the same. 
3.  Each record within the file is either defined or undefined.  A defined 
    record is one that has defined content.  An undefined record must not be 
    read. 
4.  Transfer of records to or from the file is accomplished only by 
    direct-access input/output statements. 
5.  Any record of the file may be written at any time by a direct-access 
    output statement.  Such a record becomes or remains defined. 
6.  Any defined record of the file may be read at any time by a direct-access 
    input statement. 
7.  Each record within the file is identified by an integer number.  If a file 
    consists of m records, each record is identified by one of the sequence of 
    integers 1,2,...,m.  The existence of this sequence of identifiers does 
    not imply any physical ordering of the records in the file on the external 
    medium nor does it imply that each of the m records is defined. 
A keyed access file has the following properties: 
1.  The file contains either formatted records or unformatted records, or 
    both.  It does not contain self-identified or endfile records. 
2.  The length of each record in the file is determined when that record is 
    created. 
3.  Each record within the file is either defined or undefined.  A defined 
    record is one that has been created and has defined content.  An undefined 
    record (that is, a record that does not exist) must not be read. 
4.  Transfer of records to or from the file is accomplished only by keyed 
    access input/output statements.  A keyed file can be read in sequential 
    mode. 
5.  Any record of the file may be written at any time by a keyed-access output 
    statement.  Such a record becomes or remains defined. 
6.  Any defined record of the file may be read at any time by a keyed-access 
    input statement. 
7.  Each record within the file is identified by a key value.  The physical 
    ordering of the records in the file on the external medium is system 
    dependent. 
8.  Any defined record of the file may be deleted by writing a null record (a 
    format of '( )' and an empty I/O list) with the same key value as the 
    record to be deleted. 
INTERNAL 
Internal files exist in internal storage rather than on external media.  As 
described below, an internal file takes the form of a character variable, a 
character array, or a character array element.  When used for input/output, a 
character variable or array element is called a record, and the collection of 
records is called an internal file. 
An internal file has the following properties: 
1.  The file is a character variable or a sequence of character array 
    elements. 
2.  If the file is a character variable, it consists of a single record whose 
    content, length, and state of definition are the same as the value, 
    length, and state of definition of the variable. 
3.  If the file is a sequence of character array elements, each array element 
    is a record in the file.  The records exist as a totally ordered set and 
    the ordering of the records in the file is the same as the ordering of the 
    array elements in the array.  There is a first record and a last record. 
    The length of every record in the file is the same and is equal to the 
    length of an array element in the array.  The state of definition or 
    undefinition of each record in the file is the same as that of the array 
    element that corresponds to the record. 
4.  An internal file contains only formatted records. 
5.  Transfer of records to or from the file is accomplished only by formatted 
    sequential input/output statements. 
6.  Any record written by a formatted sequential output statement becomes or 
    remains defined. 
7.  Only records that have previously been defined may be read by formatted 
    sequential input statements. 
8.  The character variable or character array element used as a storage record 
    may be defined (or undefined) by means other than an output statement. 
    For example, the variable or array element may be defined by a character 
    assignment statement. 
00004
00005
00004
00001
00002
00006
00005
00003
Syntax: 
FIND (u'r) 
Parameters: 
u    is the logical unit number.  It may be any integer, real, or double 
precision expression, truncated to integer. 
'    is a field separator. 
r    is an integer expression that points to the relative location of a record 
within the file associated with the logical unit, as defined by a DEFINE FILE 
statement. 
Description: 
The FIND statement presents the associated variable to the record number 
specified in the statement.  Its use is discouraged.  The original purpose of 
the FIND statement was to minimize wasted CPU time waiting for disk seeks on 
small, single-user systems.  The FIND statement would move the disk head to 
the cylinder containing the specified record during processing so a seek would 
not be necessary for the next READ or WRITE. 
On a large, multi-user system with many tasks competing for CPU time and the 
same disk drive, FIND serves no useful purposes.  In fact, use of FIND may 
make a program run slower, as shown in the following example: 
       DEFINE FILE 10(1000,80,U,IREC1) 
     5 FIND (10'75) 
       W1=C 
       W2=C**4 
    10 READ (10'75) A,B 
While the two statements between statements 5 and 10 are being executed, 
record 75 of the file associated with logical unit 10 is found.  When the FIND 
statement has finished executing, IREC1 contains 75; when the READ statement 
has finished, IREC1 contains 76. 
A general example containing all four elements involved in direct-access 
operations (DEFINE FILE, FIND, READ and WRITE) is shown below: 
        DEFINE FILE 10(1000,98,L,IREC1) 
        DIMENSION A(100),B(100),C(100),D(100),E(100),F(100),G(100) 
     10 FORMAT (7F14.4) 
     20 FIND (10'1) 
        DO 100 I=1,100 
    100 READ (10'IREC1+4,10)A(I),B(I),C(I),D(I),E(I),F(I),G(I) 
        . 
        . 
        . 
        DO 200 I=1,100 
    200 WRITE (10'IREC1'+4,10)A(I),B(I),C(I),D(I),E(I),F(I),G(I) 
        . 
        . 
        . 
        END 
The above example illustrates how direct-access statements collect and 
disseminate data as designated by the user.  The first DO loop fills arrays A 
through G with data from the 5th, 10th, 15th,..., and 500th record of the file 
associated with logical unit 10.  Array A receives the first value in every 
5th record, B the second value, and so on as specified by the FORMAT statement 
10 and the I/O list of the READ statement.  At the end of the READ operation, 
the records have been disseminated into arrays A through G.  At the conclusion 
of the first DO loop, IREC1 has a value of 501. 
The second DO loop in the example groups the data items from each array, as 
specified by the WRITE statement and FORMAT statement 10.  Each group of data 
items is placed in the file associated with logical unit 10.  Writing begins 
at the 505th record and continues at intervals of five until record 1000 is 
written, provided IREC1 is not changed between the last READ and the first 
WRITE. 
00002
00003
00001
FIND
The FORMAT statement is used to specify the conversion to be performed on data 
being transmitted during formatted input/output or DECODE/ENCODE operations. 
It is nonexecutable and may be placed anywhere in the program.  In general, 
conversion performed during output is the reverse of that performed during 
input. 
Syntax: 
FORMAT (S1,S2,S3,...,Sn) 
Parameters: 
n>0 
Si    is either a format specification of one of the forms described below or 
a repeated group of such specifications in the form 
r(s1,s2,s3,...,sm) 
    where 
    m>0 
    r    is a repeat count as described below. 
    sj    is as described for Si; in other words, repetitions may may be 
    nested (to ten levels). 
The commas between the Si (and sj) are optional except where ambiguities would 
arise from not separating specifications.  For example, the commas should 
always be used following an E or preceding a D Format specification due to the 
Ew.dEe and Ew.dDe Format specifications. 
In the absence of a comma, the compiler attaches as much as possible to the 
left-hand specification.  For example, the specifications 
    I23F27.13X       E14.6E3,E23.15D3 
will be interpreted as: 
    I23,F27.13,X     E14.6E3,E23.15D3 
and not as 
    I2,3F27.1,3X     E14.6,E3,E23.15,D3 
To obtain the latter interpretation, the commas are required. 
Every FORMAT statement should be labeled so that references may be made to it 
by formatted input/output statements.  An entire FORMAT (the parentheses and 
the items they enclose) may be stored in a character variable or a 
non-character array through the use of DATA statements or input statements. 
In this case, as described under FORMATS_STORED_IN_ARRAYS under the topic 
FORMATS the array itself is referenced by the input/output statements. 
00002
00001
DATA_CONVERSION_FORMATS
ADJUSTABLE_FORMATS
DATA_CONVERSION_FORMATS
PARENTHESIZED_FORMATS
Formatted input/output statements are used to process formatted records.  The 
form of conversion to be performed between the internal data and the external 
fields is specified by a FORMAT statement.  There are three kinds of formatted 
input/output statements:  sequential, direct, and keyed access. 
The simplified formatted input/output statements are written as 
  Sequential        Direct Access          Keyed Access 
  ----------        -------------          ------------- 
  READ(u,f)[k]      READ(u,f,REC=n)[k]     READ(u,f,KEY=z)[k] 
  WRITE(u,f)[k]     WRITE(u,f,REC=n)[k]    WRITE(u,f,KEY=z)[k] 
  PRINT(u,f)[k]     PRINT(u,f,REC=n)[k]    PRINT(u,f,KEY=z)[k] 
Parameters: 
u    is a logical unit number.  It may be any integer, real, or double 
precision expression, truncated to integer if necessary.  In the sequential 
case, it may also be a character variable, a character array name, or a 
character array element; in which case the character entity becomes the 
storage file and the READ or WRITE statement is interpreted as a storage file 
I/O statement. 
f    is a format reference.  It may be the statement label of a FORMAT 
statement, a variable that has had such a statement label assigned to it, the 
name of an array of a type other than character in which a format has been 
stored (see FORMATS_STORED_IN_ARRAYS under the topic FORMATS), or a character 
variable, a character array, or a character expression. 
k    is an input/output list, as previously described.  (The list may be 
omitted.) 
n    is an integer, real, or double precision expression that has a positive 
value after truncation to integer.  It denotes the record number. 
z    denotes the key value and is a character expression or an integer, real, 
or double precision expression, truncated to integer.  If a character 
expression, trailing blanks are truncated and are not included in the key 
value used in the I/O. 
Using Formatted READ and WRITE 
A formatted READ statement causes the character string in the external record 
to be converted into internal values according to the FORMAT specified.  The 
converted values are then assigned to the variables appearing in the list k 
(or the equivalent simple list, if k contains a DO implication).  Conversely, 
a formatted WRITE statement converts internal values (which may be 
expressions) into character strings and outputs them. 
Each formatted input/output statement begins processing with a new record. 
Thus, it is not possible to process a particular record using more than one 
READ or WRITE statement.  (This can be done with the DECODE and ENCODE 
statements.)  More than one record may be processed by these statements if 
specifically requested by the FORMAT statement.  However, attempting to read 
(or write) more characters on a record than are (or can be) physically present 
does not cause a new record to be begun. 
The maximum size of a formatted record depends on the record size for the 
logical unit number.  Certain devices, e.g., card punch and card reader, 
impose absolute restrictions on the size of records:  a punched card contains 
80 characters.  If a record contains no characters, it is considered to be 
blank (or empty); in other words, a record into which any number of blanks 
have been specifically written is indistinguishable within the program from an 
empty record.  Note, however, that on devices such as magnetic tape or paper 
tape, the FORMAT statement may determine the actual size of the record 
written.  The maximum size of a formatted record for disk files is 133 
characters in length.  This maximum, which is the default, can be overridden 
by use of the RECL=rl option of the FORTRAN OPEN statement. 
The list k may be omitted from a formatted input/output statement.  Normally, 
this has the effect of skipping one record (on input) or writing one blank 
record (on output).  However, if the FORMAT statement begins with Hollerith or 
slash specifications (see LIST_INTERFACE under the topic PRINT_POSITIONING) 
information may actually be processed and one or more records be used. 
Examples: 
Sequential formatted READ/WRITE statements: 
     READ(105,6)X,Y,GRUNCH,T(3,5) 
     READ(5,FORM),(A(I),I=1,40),H,Q 
     WRITE(N,FMT(L))(MASS(J,3),J=100,1,-1),A(4),B 
     WRITE(102,93)MESSAGE,ERROR NUMBER,'AT HOME' 
Direct access formatted READ/WRITE statements: 
     READ(10,7,REC=9)X,A,B,C 
     READ(21,'(F10.3,416)',REC=7)A,I 
     WRITE(19,F,REC=33)(X(I),I=1,21) 
     WRITE(K,A(4),REC=904)(Y(J),J=K,L,2) 
Keyed access formatted READ/WRITE statements: 
     READ(10,7,KEY=91)X,A,B,C 
     READ(21,'(F10.3,416)',KEY='ABLE10')A,J,K,L,M 
     WRITE(19,F,KEY=33)(X(I),I=1,21) 
     WRITE(K,A(4),KEY='SES')(Y(J),J=K,L,2) 
00002
00003
00001
READ_FORMATTED
WRITE_FORMATTED
PRINT_POSITIONING
FORMAT
CONCEPTS
Each source line of the FORTRAN program is considered as a record image.  Each 
line is divided into three fields: 
o   the label field -- columns 1 through 5 
o   the continuation field -- column 6 (except for programs entered 
    interactively at a user's terminal) 
o   the statement field -- columns 7 through 72 
However, if the FF compiler option is used, the above rules do not apply.  For 
more information type HELP (FORTRAN) COMPILER_OPTIONS FF. 
A FORTRAN-77 program consists of an ordered set of statements that describe 
both the procedure to be followed during execution of the program, and the 
data to be processed by the program.  Data values to be processed may be 
external to the program, to be read into the computer during program 
execution.  Similarly, data values generated by the program can be written out 
while processing continues.  Statements belong to one of two classes: 
1.  Executable statements, which perform computation, input/output operations, 
    and program flow control. 
2.  Nonexecutable statements, which provide information to the compiler about 
    storage assignments, data types and program form.  During execution they 
    also provide information to the program about input/output formats and 
    data initialization. 
    For additional information on FORTRAN source file format, enter: 
    HELP (FORTRAN) FORTRAN_SOURCE_INFO subtopic 
    where subtopic may be any of the following:  BLANKS, CHARACTERS_ALLOWED, 
    COMMENT_LINES, CONDITIONAL_COMPILATION, CONTINUATION_LINES, 
    FORTRAN_STATEMENTS, SAMPLE_PROGRAM, SOURCE_FILE_FORMAT. 
The statements of the FORTRAN language are used to form program units.  Each 
statement is written in columns 7 through 72 of an initial line.  If more than 
one line is required for a statement, the statement may be continued on as 
many continuation lines as needed. 
FUNCTION_STATEMENT
INTRINSICS_GENERICS
Syntax: 
[type] FUNCTION {f|f()|f(d1,d2,d3,...dn)} 
Parameters: 
f    is the identifier of the function. 
di    is a dummy argument of any of the forms described under 
ARGUMENTS_DUMMIES.  Note that no dummy arguments are required. 
type    is an optional type specification, which may be any of the following: 
     INTEGER              COMPLEX                       LOGICAL 
     INTEGER*2            DOUBLE COMPLEX (indicates     LOGICAL*1 
     REAL                  (double precision complex)   CHARACTER 
     DOUBLE PRECISION 
See also OPTIONAL_SIZE_SPECS. 
Description: 
When functions are defined as FUNCTION subprograms, they are introduced by a 
FUNCTION statement.  Values may be assigned to dummies within the FUNCTION 
subprogram with certain restrictions (see ARGUMENTS_DUMMIES). 
It is not necessary for a FUNCTION subprogram to contain a statement; 
execution of the END statement has the same function as a RETURN. 
Within the function, the identifier of a FUNCTION subprogram is treated as 
though it is a variable and, unless an alternate RETURN is taken (see the 
RETURN Statement), should be assigned a value during each execution of the 
function.  The value returned for a FUNCTION is the last one assigned to its 
identifier prior to the execution of a RETURN statement.  The variable 
corresponding to the function identifier is treated like any other variable. 
Examples: 
The following are FUNCTION statement examples: 
    DOUBLE PRECISION FUNCTION DIFFEQ(R,S,N) 
    REAL FUNCTION IOU(W,X,Y,Z1,Z2) 
    FUNCTION EXTRACT(N,A,B,C,V) 
    LOGICAL FUNCTION VERDAD(E,F,G,H,P) 
    INTEGER FUNCTION NARGUMENTS 
The following is a FUNCTION subprogram example: 
    COMPLEX FUNCTION GAMMA (Z,N) 
    COMPLEX Z 
    M=1 
    GAMMA=Z 
    DO 5 J=N,1,-1 
    M=M*J 
  5 GAMMA=GAMMA*(Z+J) 
    GAMMA=M*N**Z/GAMMA 
    END 
The identifier of a FUNCTION subprogram may appear in a type statement that 
appears within the FUNCTION subprogram.  Such a type declaration has the same 
effect as a type declaration that appears in the FUNCTION statement.  The 
statements: 
    FUNCTION F(A,B) 
    REAL F 
are equivalent to 
    REAL FUNCTION F(A,B) 
Notes on Character Functions 
If an external function has a len of (*) declared in a FUNCTION subprogram, 
the function name must appear as the name of a function in a FUNCTION or ENTRY 
statement in the same subprogram.  When a reference to such a function is 
executed, the function assumes the length specified in the referencing program 
unit. 
The length specified for a character function in the program unit that 
references the function must be an integer constant expression and should 
agree with the length specified in the subprogram that specifies the function. 
Note that there always is agreement of length if a len of (*) is specified in 
the subprogram that specifies the function. 
The length specified for a character statement function or statement function 
dummy argument of type character must be an integer constant expression. 
Note on integer *2 function.  The result of an integer *2 function is a full 
word integer value. 
00002
00003
00004
00001
LIBRARY_SUBPROGRAMS
A function is a FUNCTION subprogram that is referenced as a basic element in 
an expression.  A function acts upon its arguments, if any, and produces a 
single datum called the function value.  The appearance of a function 
reference constitutes a reference to the value produced by the function, when 
operating on the given arguments. 
A function reference is denoted by the identifier that names the function, 
followed by a list of arguments enclosed in parentheses: 
f(a1,a2,...,an) 
where 
f    is the identifier of the function. 
ai    is an argument.  Arguments may be constants, variables, array elements, 
expressions, statement labels, or array or subprogram names (see 
ARGUMENTS_DUMMIES).  The argument list may be null. 
The data type of a function (except for a generic function reference and some 
intrinsic functions) is determined in the same way as for an identifier; that 
is, unless the type of the function identifier is specifically declared 
otherwise, the IJKLMN rule applies, along with whatever variations have been 
affected by the IMPLICIT statement (if any).  (See FUNCTION_TYPES 
INTRINSIC_FUNCTIONS.) 
Examples of function references are 
    SIN(A+B)     CHECK(7.3,J,ABS(Y))     KOST(ITEMNUM) 
Many intrinsic functions are provided in FORTRAN-77. 
INTRINSICS_GENERICS
Syntax: 
GLOBAL v1,v2,v3,...,vn 
Parameters: 
each vi    is either a variable identifier or an array identifier or an array 
declarator. 
Description: 
Like the COMMON statement, the GLOBAL statement is used to permit two or more 
programs to access the same data.  Unlike the COMMON statement, however, the 
GLOBAL statement does not depend on the ordering of variables by name. 
Normally, the use of a variable identifier in more than one program unit does 
not cause access of the same data.  However, when an identifier is placed in 
GLOBAL declarations in more than one program unit, all references to the 
identifier in those program units access the identical data items and the 
identifier must be classified identically in those programs (e.g., dimensions 
and data type). 
Each variable identifier appearing in a GLOBAL declaration defines a discrete 
GLOBAL block of the data type and size defined for the variable. 
Examples: 
    GLOBAL DDG,BVE(0:45),H,U(55,55,100) 
    GLOBAL WARFARE,VANLINES 
    GLOBAL ONE(2,3,4),FIVE,SIX(7),HACHI,NEIN 
GLOBAL variable names must be unique with respect to 
1.  Subprogram names defined explicitly or implicitly to be external 
    references. 
2.  Other GLOBAL variables. 
3.  Labeled COMMON block names. 
4.  Other variable or subprogram identifiers (including dummy identifiers) 
    appearing in the same program as the GLOBAL identifier. 
00002
00001
Syntax: 
GO TO i[[,](s[,s]...)] 
Parameters: 
i    is an integer variable name. 
s    is the statement label of an executable statement that appears in the 
same program unit as the assigned GO TO statement.  The same statement label 
may appear more than once in the same assigned GO TO statement. 
Description: 
At the time of execution of an assigned GO TO statement, the variable i must 
be defined with the value of a statement label of an executable statement that 
appears in the same program unit.  Note that the variable may be defined with 
a statement label value only by an ASSIGN statement in the same program unit 
as the assigned GO TO statement.  The execution of the assigned GO TO 
statement causes a transfer of control so that the statement identified by 
that statement label is executed next. 
If the optional form is used (i.e., the lists), each label appearing in the 
list must be the label of an executable statement.  This form serves only to 
provide compatibility with other systems. 
Examples: 
    ASSIGN 5371 TO LOC               ASSIGN 23 TO M(J) 
    GO TO LOC                        GO TO M(J) 
    GO TO LOC,(117,56,101,5371)      GO TO M(J),(88,75,54,23) 
The first two GO TO statements transfer control to the statement labeled 5371. 
The second two GO TO statements transfer control to the statement labeled 23. 
00002
00003
00001
Syntax: 
GO TO (k1, k2, k3,...kn),e 
Parameters: 
ki    is a statement label. 
e    is an expression of integer, real, or double precision data type whose 
value (truncated to integer if necessary) determines which of the ki to 
transfer control to.  The comma preceding e is optional. 
n    is greater than or equal to 1. 
Description: 
This statement causes control to be transferred to the statement whose label 
is kj, where j is the integer value of the expression e, for 1<j<n.  If j is 
not between 1 and n, no transfer occurs, and control passes to the statement 
following the computed GO TO statement. 
Examples: 
    Statement                   Expression Value      Transfer to 
    GO TO (98,12,405,3),N             3                   405 
    GO TO (1,8,7,562),Q**2+2          2.81450             8 
    GO TO (4,88,1),N*MAX(K,MB/3)      0                   next statement 
    GO TO (63,9,777,3,2),A+1          45201.7             next statement 
00002
00003
00001
GO TO statements transfer control unconditionally from one point in a program 
to another.  FORTRAN-77 includes three forms of the GO TO statement: 
    UNCONDITIONAL_GO_TO 
    ASSIGNED_GO_TO 
    COMPUTED_GO_TO 
Syntax: 
GO TO k 
Parameters: 
k    is a statement label.  The result of the execution of this statement is 
that the next statement executed is the statement whose label is k. 
Description: 
The unconditional GO TO statement transfers control from where it occurs in a 
program to the statement whose label is k. 
Examples: 
          . 
          . 
          . 
          GO TO 502 
    98    X = Y 
          . 
          . 
          . 
    502   A = B 
the statement with the label 502 will be executed immediately after the GO TO 
statement. 
00002
00003
00001
    The CP-6 HELP facility provides information concerning the CP-6 operating 
    system and the most essential software processors.  General conceptual and 
    specific command information can be requested.  HELP messages can include: 
        o   Command Rules 
        o   System/processor Interactions 
        o   User Notes 
        o   Syntaxes 
        o   Parameter Descriptions 
        o   Command Examples 
        o   Lists of Related HELP Information 
Syntax: 
HELP [(processor)] [TOPICS] [keyword1] [-] [keyword2] 
NOTE:  The following elements can be specified in any order: 
       (processor) 
       TOPICS 
       [keyword1] [-] [keyword2] 
For example, HELP (processor) keyword1 - keyword2 TOPICS is acceptable. 
Parameters: 
processor    specifies a program or processor that has an associated HELP 
file, such as FORTRAN. 
TOPICS    requests a list of topic or subtopic names, rather than an 
information message. 
Form                                     Result 
--------------------------               -------------------------- 
HELP (fid) TOPICS                        Lists all topics 
HELP (fid) TOPICS keyword1 - keyword2    Lists all topics in the range 
                                         specified by keyword1 - keyword 2 
HELP (fid) TOPICS keyword1?              Lists all topics beginning with 
                                         the prefix specified by keyword1 
HELP (fid) TOPICS keyword1               Lists all subtopics for the 
                                         topic specified by keyword1 
keyword1 [- [keyword2]]    specifies a topic, a range of topics, or a topic 
and subtopic to identify what HELP information is requested. 
Form                                       Result 
--------------------------               -------------------------- 
HELP (fid) keyword1                        Displays the first level 
                                           information message for 
                                           the topic keyword1 
HELP (fid) keyword1 keyword2               Displays the information 
                                           message for keyword1, but 
                                           only the level identified 
                                           by the subtopic keyword2 
HELP (fid) TOPICS keyword1 [-] [keyword2]  Lists topic or subtopic 
                                           names.  See TOPICS parameter. 
keyword1 may include the wildcard (?) character as the rightmost character, if 
TOPICS is specified. 
Description: 
HELP displays information.  The HELP command can be used either within a 
processor or in IBEX (at the ! prompt).  HELP messages have levels.  Once the 
initial level has been displayed, typing a question mark prints the next 
level, usually containing greater detail.  Entering two question marks 
displays the entire message. 
Examples: 
    !HELP (FORTRAN) TOPICS 
means print the names of all the messages available for the FORTRAN processor. 
    !HELP (FORTRAN) TOPICS A - C 
means print the names of messages between A and C in the FORTRAN HELP file. 
    !HELP (FORTRAN) A - C 
means print all the messages between A and C in the FORTRAN HELP file. 
    !? 
means print the next deeper level of the current message. 
    !?? 
means print all levels of the current message. 
    !HELP (FORTRAN) OVERVIEW 
means print the FORTRAN Overview message, which presents only the major 
FORTRAN HELP topics. 
00003
00004
00002
00001
A 
  ABNORMAL Statement - 
      HELP ABNORMAL_STATEMENT 
  ABORTSET - 
      HELP ABORTSET 
  Additional Library Subprograms - 
      HELP ADDITIONAL_LIBRARY_SUBPROGRAMS 
  Adjustable Dimensions for Dummy Arrays - 
      HELP DUMMY_ARRAY_DIMENSIONS 
  Adjustable Format Specifications - 
      HELP ADJUSTABLE_FORMATS 
  Allocation of Variables - 
      HELP VARIABLE_ALLOCATION 
  Alternate Direct Access READ Statement - 
      HELP READ_ALTERNATE_DIRECT_ACCESS 
  Alternate Direct Access WRITE Statement - 
      HELP WRITE_ALTERNATE_DIRECT_ACCESS 
  Alternate NAMELIST Statement - 
      HELP NAMELIST_ALTERNATE_STATEMENT 
  ANS Interpretation of RETURN e - 
      HELP ANS_RETURN_E 
  Arguments and Dummies - 
      HELP ARGUMENTS_DUMMIES 
  Arithmetic Expressions - 
      HELP ARITHMETIC_EXPRESSIONS 
  Arithmetic IF Statement - 
      HELP IF_ARITHMETIC 
  Arithmetic Overflow - 
      HELP ARITHMETIC_OVERFLOW 
  Arrangement of COMMON - 
      HELP COMMON_BLOCK_ARRANGEMENT 
  Array Block Designator - 
      HELP ARRAY_BLOCK_DESIGNATOR 
  Array Declarators - 
      HELP ARRAY_DECLARATORS 
  Array Storage - 
      HELP DATA_CONVERSION_FORMATS COMMA_FIELD_TERMINATION 
  Array Storage - 
      HELP ARRAY_STORAGE 
  Arrays -   
    formats stored in - 
      HELP DATA_CONVERSION_FORMATS COMMA_FIELD_TERMINATION 
    integer - 
      HELP DATA_CONVERSION_FORMATS INTEGER_ARRAYS 
  ASSIGN (Statement Label) Statement - 
      HELP STATEMENT_LABEL_ASSIGNMENT 
  Assigned GO TO Statement - 
      HELP GO_TO_ASSIGNED 
  Assignment Statement -   
    Extended - 
      HELP EXTENDED_ASSIGNMENT 
    Simple - 
      HELP ASSIGNMENT_STATEMENTS 
  Assignment Statement - 
      HELP ARITHMETIC_OVERFLOW 
  assignment types (mixed) - 
      HELP MIXED_ASSIGNMENT_TYPES 
  Automatic Double Precision - 
      HELP AUTOMATIC_DOUBLE_PRECISION 
  Auxiliary Input/Output Statements - 
      HELP AUXILIARY_IO_STATEMENTS 
B 
  BACKSPACE Statement - 
      HELP BACKSPACE 
  Blank COMMON - 
      HELP BLANK_COMMON 
  Blanks - 
      HELP BLANKS 
  BLOCK DATA Statement - 
      HELP BLOCK_DATA_STATEMENT 
  Block Data Subprograms - 
      HELP BLOCK_DATA_SUBPROGRAMS 
  Block IF Statement - 
      HELP IF_BLOCK 
  Boolean Operations - 
      HELP BOOLEAN_OPERATIONS 
  BUFFERIN - 
      HELP BUFFIN_BUFFERIN 
  BUFFIN and BUFFERIN - 
      HELP BUFFIN_BUFFERIN 
  BUFFOUT and BUFFEROUT - 
      HELP BUFFOUT_BUFFEROUT 
C 
  CALL DIRECT WRITE and CALL DIRECT READ - 
      HELP WRITE_DIRECT 
  CALL Statement - 
      HELP CALL 
  Changes/New Features in this Release - 
      HELP CHANGES 
  Character Expressions - 
      HELP CHARACTER_EXPRESSIONS 
  Character Manipulation in FORTRAN using Numeric Data - 
      HELP CHARACTER_MANIPULATION 
  CHARACTER statement - 
      HELP PARAMETER_STATEMENT EXAMPLES: 
  CHARACTER statement - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
  CHARACTER statement - 
      HELP EXPLICIT_TYPE_STATEMENTS PARAMETERS: 
  Character Substring - 
      HELP CHARACTER_SUBSTRING 
  Characters - 
      HELP CHARACTERS 
  Characters Acceptable in Column 1 - 
      HELP COLUMN_1 
  Classification of Identifiers - 
      HELP CLASSIFICATION_OF_IDENTIFIERS 
  CLOSE Statement - 
      HELP CLOSE 
  Collating Sequence - 
      HELP COLLATING_SEQUENCE 
  Comment Lines - 
      HELP COMMENTS 
  COMMON Statement - 
      HELP COMMON_STATEMENT 
  compilation -   
    FF option - 
      HELP FORTRAN_SOURCE_FILE_FORMAT SOURCE_FILE_FORMAT 
    option groups - 
      HELP COMPILER_OPTION_GROUPS 
  COMPLEX statement - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
      HELP EXPLICIT_TYPE_STATEMENTS PARAMETERS: 
  Compound Statements - 
      HELP COMPOUND_STATEMENTS 
  Computed GO TO Statement - 
      HELP GO_TO_COMPUTED 
  Conditional Compilation - 
      HELP CONDITIONAL_COMPILATION 
  CONSTANT Statement - 
      HELP CONSTANT_STATEMENT 
  Constants -   
    alternate forms - 
      HELP CONSTANTS EXAMPLES: 
    character - 
      HELP CONSTANTS CHARACTER 
    complex - 
      HELP CONSTANTS COMPLEX 
    double precision - 
      HELP CONSTANTS DOUBLE_PRECISION 
    double precision complex - 
      HELP CONSTANTS DOUBLE_PRECISION_COMPLEX 
    Hexadecimal - 
      HELP CONSTANTS HEXADECIMAL 
    Hollerith - 
      HELP CONSTANTS HOLLERITH 
    integer - 
      HELP CONSTANTS INTEGER 
    literal - 
      HELP CONSTANTS LITERAL 
    logical - 
      HELP CONSTANTS LOGICAL 
    octal - 
      HELP CONSTANTS OCTAL 
    real - 
      HELP CONSTANTS REAL 
  Constants - 
      HELP CONSTANTS 
      HELP CONSTANTS INTEGER 
  continuation field - 
      HELP FORTRAN_SOURCE_FILE_FORMAT SOURCE_FILE_FORMAT 
  Continuation Lines - 
      HELP CONTINUATION_LINES 
  continuation mark - 
      HELP CONTINUATION_LINES CONTINUATION_LINES 
  CONTINUE Statement - 
      HELP CONTINUE 
  Control Statements - 
      HELP EXTENDED_ASSIGNMENT EXAMPLES: 
  CP-V PARAMETER Statement - 
      HELP CPV_PARAMETER_STATEMENT 
D 
  Data Constant List - 
      HELP DATA_CONSTANT_LIST 
  Data Conversion Formats - 
      HELP DATA_CONVERSION_FORMATS 
  DATA Statement - 
      HELP DATA_STATEMENT 
  Data Statement - 
      HELP DATA_CONSTANT_LIST PARAMETERS: 
  data type correspondence - 
      HELP DATA_CONSTANT_LIST PARAMETERS: 
  Data Types - 
      HELP DATA_TYPES 
  Data Variable List - 
      HELP DATA_VARIABLE_LIST 
  Declaration Statements - 
      HELP PRINT_POSITIONING CARRIAGE_CONTROL_PRINT_OUTPUT 
  DECODE Statement - 
      HELP DECODE 
  Default Options - 
      HELP DEFAULT_OPTIONS 
  DEFINE FILE Statement - 
      HELP DEFINE_FILE 
  Device Format Control characters - 
      HELP VFCP6 EXAMPLE: 
  DIMENSION Statement - 
      HELP DIMENSION_STATEMENT 
  DO Loop - 
      HELP DO DESCRIPTION: 
  DO Statement - 
      HELP DO 
  DO TIMES Statement - 
      HELP DO_TIMES 
      HELP DO_TIMES EXAMPLE: 
  DO WHILE Statement - 
      HELP DO_WHILE 
      HELP DO_WHILE DESCRIPTION: 
  DO-Implied Lists - 
      HELP DO_IMPLIED_LISTS 
  DOUBLE COMPLEX statement - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
      HELP EXPLICIT_TYPE_STATEMENTS PARAMETERS: 
  DOUBLE PRECISION statement - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
      HELP EXPLICIT_TYPE_STATEMENTS PARAMETERS: 
  Doubleword Variable Rules - 
      HELP DOUBLEWORD_VARIABLE_RULES 
  Dummy Arrays - 
      HELP DUMMY_ARRAYS 
  Dummy Subprograms - 
      HELP DUMMY_SUBPROGRAMS 
  Dummy Variables - 
      HELP DUMMY_VARIABLES 
  DUMP, PDUMP Subprogram (Memory Dump) - 
      HELP DUMP_PDUMP 
E 
  ELSE IF block - 
      HELP IF_ELSE ELSE_IF_BLOCK 
  ELSE IF Statement - 
      HELP IF_ELSE 
  ELSE Statement - 
      HELP ELSE 
  Embedded Comments - 
      HELP EMBEDDED_COMMENTS 
  ENCODE Statement - 
      HELP ENCODE 
  END DO Statement - 
      HELP ENDDO 
      HELP ENDDO EXAMPLE: 
  END IF Statement - 
      HELP END_IF 
  END Statement - 
      HELP END 
  End-of-File Processing - 
      HELP END_OF_FILE_PROCESSING 
  ENDFILE Statement - 
      HELP ENDFILE 
  ENTRY Statement - 
      HELP ENTRY_STATEMENT 
  EOFSET - 
      HELP EOFSET 
  EQUIVALENCE Statement - 
      HELP EQUIVALENCE_STATEMENT 
  ERROROUT - 
      HELP ERROROUT 
  executable statements - 
      HELP WRITE_DIRECT USAGE_NOTES: 
  EXIT Statement - 
      HELP EXIT 
      HELP EXIT EXAMPLE: 
  EXIT Subprogram - 
      HELP EXIT_SUBPROGRAM 
  Explicit Declarations - 
      HELP EXPLICIT_DECLARATIONS 
  explicit declarations - 
      HELP EXPLICIT_DECLARATIONS 
  Explicit Type Statement - 
      HELP EXPLICIT_TYPE_STATEMENTS 
  Expression -   
    character - 
      HELP CHARACTER_EXPRESSIONS 
    logical - 
      HELP LOGICAL_EXPRESSIONS 
    mixed - 
      HELP MIXED_EXPRESSIONS 
    relational - 
      HELP RELATIONAL_EXPRESSIONS 
  Expression - 
      HELP MIXED_EXPRESSIONS 
  Expressions - 
      HELP VIRTUAL_ARRAYS 
  EXTERNAL Statement - 
      HELP EXTERNAL_STATEMENT 
F 
  Files -   
    direct-access - 
      HELP FILES DIRECT_ACCESS 
    internal - 
      HELP FILES INTERNAL 
    keyed access - 
      HELP FILES KEYED_ACCESS 
    sequential - 
      HELP FILES SEQUENTIAL 
  Files - 
      HELP FILES 
      HELP FILES SEQUENTIAL 
  FIND Statement - 
      HELP FIND 
  Format Specifications - 
      HELP EOFSET 
  Format Statement - 
      HELP FORMAT 
  Formatted Input/Output Statements - 
      HELP FORMATTED_IO_STATEMENTS 
  FORTRAN HELP Facility - 
      HELP HELP_FACILITY 
  FORTRAN Source File Format - 
      HELP FORTRAN_SOURCE_FILE_FORMAT 
  FORTRAN Source Listing Format Declarators - 
      HELP SOURCE_DECLARATORS 
  FORTRAN Statements - 
      HELP FORTRAN_STATEMENTS 
  FORTRAN-77 Compilation Options - 
      HELP COMPILER_OPTIONS 
  FORTRAN-77 Concepts - 
      HELP CONCEPTS 
  FUNCTION Statement - 
      HELP FUNCTION_STATEMENT 
  Function Values - 
      HELP FUNCTION_VALUES 
G 
  GLOBAL Statement - 
      HELP GLOBAL_STATEMENT 
  GO TO statement -   
    assigned - 
      HELP GO_TO_ASSIGNED 
    computed - 
      HELP GO_TO_COMPUTED 
    unconditional - 
      HELP GO_TO_UNCONDITIONAL 
  GO TO Statement - 
      HELP GO_TO_STATEMENTS 
  GO TO Statements - 
      HELP GO_TO_STATEMENTS 
H 
  HELP - 
      HELP HELP_FACILITY 
I 
  ICHECK - 
      HELP ICHECK 
  Identifiers - 
      HELP IDENTIFIERS 
  identifiers, classification of - 
      HELP CLASSIFICATION_OF_IDENTIFIERS 
  IF Statement -   
    arithmetic - 
      HELP IF_ARITHMETIC 
    block - 
      HELP IF_BLOCK 
    logical - 
      HELP IF_LOGICAL 
  IF Statement - 
      HELP IF 
  IF Statements - 
      HELP IF 
  IF Subprogram (Compare for Approximately Equal/Zero) - 
      HELP IF_SUBPROGRAM_COMPARE 
  Implicit Declarations - 
      HELP IMPLICIT_DECLARATIONS 
  IMPLICIT Statement -   
    (and functions) - 
      HELP FUNCTION_VALUES 
    and PARAMETER statement - 
      HELP PARAMETER_STATEMENT DESCRIPTION: 
  IMPLICIT Statement - 
      HELP IMPLICIT_STATEMENT 
  implicit type statement - 
      HELP IMPLICIT_STATEMENT SYNTAX: 
  INCLUDE Statement - 
      HELP INCLUDE 
  Input controls - 
      HELP COMPILER_OPTION_GROUPS 
  INPUT Statement Using NAMELIST (Form 2 of INPUT) - 
      HELP INPUT_FORM2 
  INPUT Statement with List (Form 1 of INPUT) - 
      HELP INPUT_FORM1 
  Input/Output -   
    Auxiliary I/O Statements - 
      HELP AUXILIARY_IO_STATEMENTS 
    Formatted I/O Statements - 
      HELP FORMATTED_IO_STATEMENTS 
    Internal File I/O - 
      HELP INTERNAL_FILE_IO 
    List Directed I/O Statements - 
      HELP LIST_DIRECTED_IO_STATEMENTS 
    Lists - 
      HELP IO_LISTS 
    supplementary - 
      HELP IO_SPECIFIERS END_OF_FILE 
    Unformatted I/O Statements - 
      HELP UNFORMATTED_IO_STATEMENTS 
    Unit Assignments - 
      HELP IO_UNIT_ASSIGNMENTS 
  Input/Output - 
      HELP END EXAMPLES: 
  Input/Output Lists - 
      HELP IO_LISTS 
  Input/Output Statement Specifiers - 
      HELP IO_SPECIFIERS 
  Input/Output Unit Assignments - 
      HELP IO_UNIT_ASSIGNMENTS 
  INQUIRE by File Statement (Form 1 of INQUIRE) - 
      HELP INQUIRE_BY_FILE 
  INQUIRE by Unit Statement (Form 2 of INQUIRE) - 
      HELP INQUIRE_BY_UNIT 
  INQUIRE Specifiers for Either Form - 
      HELP INQUIRE_SPECIFIERS 
  INQUIRE Statement - 
      HELP INQUIRE 
  INTEGER statement - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
      HELP EXPLICIT_TYPE_STATEMENTS PARAMETERS: 
  Interactions of Storage Allocation Statements - 
      HELP STORAGE_ALLOCATION_RULES 
  Internal File Input/Output - 
      HELP INTERNAL_FILE_IO 
  Internal File READ Statement - 
      HELP INTERNAL_FILE_READ 
  Internal File WRITE Statement - 
      HELP INTERNAL_FILE_WRITE 
  INTRINSIC Statement - 
      HELP INTRINSIC_STATEMENT 
  ios - 
      HELP ERROROUT DESCRIPTION: 
  IOSTAT - 
      HELP ERROROUT DESCRIPTION: 
L 
  label field - 
      HELP FORTRAN_SOURCE_FILE_FORMAT SOURCE_FILE_FORMAT 
  Labeled COMMON - 
      HELP LABELED_COMMON 
  Labels - 
      HELP LABELS 
  Library Subprograms - 
      HELP LIBRARY_SUBPROGRAMS 
  List Directed Input/Output Statements - 
      HELP LIST_DIRECTED_IO_STATEMENTS 
  List-Directed Input Record - 
      HELP LIST_DIRECTED_INPUT_RECORD 
  List-Directed Output Record - 
      HELP LIST_DIRECTED_OUTPUT_RECORD 
  Lists, input/output - 
      HELP IO_LISTS 
  Logical Expressions - 
      HELP LOGICAL_EXPRESSIONS 
  Logical IF Statement - 
      HELP IF_LOGICAL 
  Logical Operators - 
      HELP LOGICAL_OPERATORS 
  LOGICAL statement - 
      HELP PARAMETER_STATEMENT EXAMPLES: 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
      HELP EXPLICIT_TYPE_STATEMENTS PARAMETERS: 
M 
  Main Programs - 
      HELP MAIN_PROGRAMS 
  Memory-to-Memory Data Conversion - 
      HELP ENCODE_DECODE 
  mixed assignment types - 
      HELP MIXED_ASSIGNMENT_TYPES 
  Mixed Expressions - 
      HELP MIXED_EXPRESSIONS 
  Multiple Entry into a Subprogram - 
      HELP MULTIPLE_ENTRY_INTO_SUBPROGRAM 
N 
  NAMELIST name - 
      HELP NAMELIST_ALTERNATE_STATEMENT PARAMETERS: 
  NAMELIST Statement -   
    alternate form - 
      HELP NAMELIST_ALTERNATE_STATEMENT 
    types - 
      HELP NAMELIST_TYPES 
  NAMELIST Statement - 
      HELP NAMELIST_STATEMENT 
  NAMELIST Statement Types - 
      HELP NAMELIST_TYPES 
  NAMELIST WRITE OUTPUT Data - 
      HELP NAMELIST_WRITE_OUTPUT_DATA 
  Names (symbolic) - 
      HELP SYMBOLIC_NAMES 
  Nested DO Loops - 
      HELP NESTED_DO_LOOPS 
  Nested Subexpressions - 
      HELP NESTED_SUBEXPRESSIONS 
  Nesting of Relational Expressions - 
      HELP NESTING_RELATIONAL_EXPRESSIONS 
  NORMAL Statement - 
      HELP NORMAL_STATEMENT 
O 
  OPEN Statement - 
      HELP OPEN 
  option (FF) - 
      HELP FORTRAN_SOURCE_FILE_FORMAT SOURCE_FILE_FORMAT 
  Option Groups - 
      HELP COMPILER_OPTION_GROUPS 
  Optional Size Specifications - 
      HELP OPTIONAL_SIZE_SPECS 
  Output controls - 
      HELP COMPILER_OPTION_GROUPS 
  OUTPUT Statement - 
      HELP OUTPUT 
  OVERFL and DVCHK Subprograms (Floating Overflow Test) - 
      HELP OVERFL_DVCHK 
P 
  PARAMETER Statement -   
    and IMPLICIT statement - 
      HELP PARAMETER_STATEMENT DESCRIPTION: 
    and type statements - 
      HELP PARAMETER_STATEMENT DESCRIPTION: 
  PARAMETER Statement - 
      HELP PARAMETER_STATEMENT 
  PARAMETER Statement - 
      HELP CPV_PARAMETER_STATEMENT 
  Parenthesized Format Specifications - 
      HELP PARENTHESIZED_FORMATS 
  PAUSE Statement - 
      HELP PAUSE 
  Precision Compilation Option - 
      HELP PRECISION_OPTION 
  Print Positioning, Scaling, and Editing - 
      HELP PRINT_POSITIONING 
  PRINT Statement -   
    formatted - 
      HELP FORMATTED_IO_STATEMENTS 
    list directed - 
      HELP LIST_DIRECTED_IO_STATEMENTS 
  PRINT Statement - 
      HELP STANDARD_UNIT_ASSIGNMENTS 
      HELP ENCRYPTION_FOR_FORTRAN_DATA_RECORDS 
  Printing of Formatted Records - 
      HELP PRINTING_FORMATTED_RECORDS 
  PROGRAM Statement - 
      HELP PROGRAM_STATEMENT 
  program-generation control - 
      HELP COMPILER_OPTION_GROUPS 
  Programs - 
      HELP FORTRAN_SOURCE_INFO 
  PROMPT - 
      HELP PROMPT 
  PUNCH Statement - 
      HELP STANDARD_UNIT_ASSIGNMENTS 
R 
  Random Word Access Statements - 
      HELP RANDOM_WORD_ACCESS_STATEMENTS 
  READ DISK Statement - 
      HELP RANDOM_WORD_ACCESS_STATEMENTS 
  READ Statement -   
    alternate direct access - 
      HELP READ_ALTERNATE_DIRECT_ACCESS 
    formatted - 
      HELP FORMATTED_IO_STATEMENTS 
      HELP PRINT_POSITIONING LIST_INTERFACE 
    internal file - 
      HELP INTERNAL_FILE_IO 
      HELP INTERNAL_FILE_READ 
    list directed - 
      HELP LIST_DIRECTED_IO_STATEMENTS 
    unformatted - 
      HELP UNFORMATTED_IO_STATEMENTS 
  READ Statement - 
      HELP STANDARD_UNIT_ASSIGNMENTS 
  REAL statement - 
      HELP PARAMETER_STATEMENT EXAMPLES: 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
      HELP EXPLICIT_TYPE_STATEMENTS PARAMETERS: 
  Records -   
    endfile - 
      HELP RECORDS ENDFILE 
    formatted - 
      HELP RECORDS FORMATTED 
    free-form - 
      HELP RECORDS FREE_FORM 
    self-identified - 
      HELP RECORDS SELF_IDENTIFIED 
    unformatted - 
      HELP RECORDS UNFORMATTED 
  Records - 
      HELP RECORDS 
      HELP RECORDS UNFORMATTED 
  Reference to Actual Arguments - 
      HELP REF_TO_ACTUAL_ARGUMENTS 
  Relational Expression - 
      HELP RELATIONAL_EXPRESSIONS 
  Relational Operators - 
      HELP RELATIONAL_OPERATORS 
  REPEAT FOR Statement - 
      HELP REPEAT_FOR 
  REPEAT WHILE Statement - 
      HELP REPEAT_WHILE 
  RETURN Statement - 
      HELP RETURN_FORM1 
  RETURN Statement (Form 2) - 
      HELP RETURN_FORM2 
  REWIND Statement - 
      HELP REWIND 
  Rule of Type Correspondence - 
      HELP RULE_OF_TYPE_CORRESPONDENCE 
S 
  Sample Program - 
      HELP SAMPLE_PROGRAM 
  SAVE Statement - 
      HELP SAVE_STATEMENT 
  Simplified Input/Output - 
      HELP SIMPLIFIED_IO 
  SLITE Subprogram (Set Sense Light) - 
      HELP SLITE 
  SLITET Subprogram (Sense Light Test) - 
      HELP SLITET 
  SSWITCH/SSWTCH Subprogram (Sense Switch Test) - 
      HELP SSWITCH 
  statement field - 
      HELP FORTRAN_SOURCE_FILE_FORMAT SOURCE_FILE_FORMAT 
  Statement Function Definition Statement - 
      HELP STATEMENT_FUNCTION_DEFINITION 
  Statement Label Argument - 
      HELP STATEMENT_LABEL_ARGUMENT 
  Statement Label Assignment (ASSIGN) Statement - 
      HELP STATEMENT_LABEL_ASSIGNMENT 
  statement typing -   
    CHARACTER statement - 
      HELP PARAMETER_STATEMENT EXAMPLES: 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    COMPLEX statement - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    DOUBLE COMPLEX statement - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    DOUBLE PRECISION statement - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    INTEGER statement - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    LOGICAL statement - 
      HELP PARAMETER_STATEMENT EXAMPLES: 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    REAL statement - 
      HELP PARAMETER_STATEMENT EXAMPLES: 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
  statement typing - 
      HELP PARAMETER_STATEMENT EXAMPLES: 
  Statements -   
    executable - 
      HELP WRITE_DIRECT USAGE_NOTES: 
  Statements - 
      HELP WRITE_DIRECT USAGE_NOTES: 
  STOP Statement - 
      HELP STOP 
  Storage Allocation Statements - 
      HELP STORAGE_ALLOCATION_STATEMENTS 
  Subprograms - 
      HELP SUBPROGRAMS 
  SUBROUTINE Statement - 
      HELP SUBROUTINE_STATEMENT 
  SUBROUTINE Subprograms - 
      HELP SUBROUTINE_SUBPROGRAMS 
  Subscripts - 
      HELP SUBSCRIPTS 
  Supplementary Input/Output - 
      HELP IO_SPECIFIERS END_OF_FILE 
  Symbolic Names - 
      HELP SYMBOLIC_NAMES 
T 
  TRUNCATE Subroutine - 
      HELP TRUNCATE 
  type -   
    explicit - 
      HELP EXPLICIT_TYPE_STATEMENTS 
    implicit - 
      HELP IMPLICIT_STATEMENT SYNTAX: 
      HELP IMPLICIT_STATEMENT EXAMPLES: 
  type - 
      HELP IMPLICIT_STATEMENT EXAMPLES: 
  type statements -   
    and PARAMETER statement - 
      HELP PARAMETER_STATEMENT DESCRIPTION: 
    CHARACTER statement - 
      HELP PARAMETER_STATEMENT EXAMPLES: 
    CHARACTER statement - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    COMPLEX - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    DOUBLE COMPLEX - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    DOUBLE PRECISION - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    INTEGER - 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    LOGICAL statement - 
      HELP PARAMETER_STATEMENT EXAMPLES: 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
    REAL statement - 
      HELP PARAMETER_STATEMENT EXAMPLES: 
      HELP IMPLICIT_STATEMENT PARAMETERS: 
  type statements - 
      HELP PARAMETER_STATEMENT DESCRIPTION: 
U 
  Unconditional GO TO Statement - 
      HELP GO_TO_UNCONDITIONAL 
  Unformatted Input/Output Statements - 
      HELP UNFORMATTED_IO_STATEMENTS 
V 
  Variables - 
      HELP VARIABLES 
  vector subroutines - 
      HELP VECTOR_SUBROUTINES VADD 
  VFC Vertical Format Control - 
      HELP VFCP6 EXAMPLE: 
  VFCP6 Subroutine - 
      HELP VFCP6 
  VIRTUAL and EQUIVALENCE Interactions - 
      HELP VIRTUAL_EQUIVALENCE 
  Virtual Array Storage - 
      HELP VIRTUAL_ARRAY_STORAGE 
  Virtual Arrays -   
    COMMON statement - 
      HELP ARRAY_DECLARATORS USAGE_NOTES: 
    DIMENSION statement - 
      HELP ARRAY_DECLARATORS USAGE_NOTES: 
    GLOBAL statement - 
      HELP ARRAY_DECLARATORS USAGE_NOTES: 
    storage - 
      HELP VIRTUAL_ARRAY_STORAGE 
  Virtual Arrays - 
      HELP VIRTUAL_ARRAYS 
  virtual arrays - 
      HELP ARRAY_DECLARATORS USAGE_NOTES: 
  Virtual Arrays with OPEN, CLOSE and INQUIRE Statements - 
      HELP VIRTUAL_OPEN_CLOSE_INQUIRE 
  VIRTUAL Statement - 
      HELP VIRTUAL_STATEMENT 
W 
  WRITE DISK Statement - 
      HELP RANDOM_WORD_ACCESS_STATEMENTS 
  WRITE Statement -   
    alternate direct access - 
      HELP WRITE_ALTERNATE_DIRECT_ACCESS 
    formatted - 
      HELP FORMATTED_IO_STATEMENTS 
      HELP PRINT_POSITIONING LIST_INTERFACE 
    internal file - 
      HELP INTERNAL_FILE_IO 
    list directed - 
      HELP LIST_DIRECTED_IO_STATEMENTS 
    unformatted - 
      HELP UNFORMATTED_IO_STATEMENTS 
  WRITE Statement - 
      HELP STANDARD_UNIT_ASSIGNMENTS 
00001
00002
00003
00004
00005
00006
00007
00008
00009
00010
00011
00012
00013
00014
00015
00016
00017
00018
00019
HELP_INDEX
Syntax: 
ICHECK(u) 
   or 
ICHECK(u,i) 
   or 
ICHECK(u,i,n) 
   or 
ICHECK(u,i,n,k) 
Parameters: 
u    is an integer expression that specifies the logical unit number of the 
device on which the operation is to be performed. 
i    is an integer variable into which an indication of the status of the 
operation is stored.  The status is indicated as follows: 
    1 - Operation incomplete (only possible with BUFFIN/BUFFOUT). 
    2 - Successful completion; no errors. 
    3 - End-of-file, end-of-tape, or other abnormal condition. 
        encountered. 
    4 - Operation complete but read error, write error, or other 
        error condition occurred. 
n    is optional, but when specified is an integer variable into which is 
stored the number of words actually input or output.  See BUFFIN for further 
details concerning this parameter. 
k    is optional, but when specified is an integer variable into which is 
stored the abnormal or error code, if any, as supplied by the monitor.  See 
BUFFIN for further details concerning this parameter. 
Description: 
For BUFFIN and BUFFOUT, the indicator (i), the count (n), and the code (k) are 
not returned until the user specifically checks their status with a call on 
ICHECK.  This can be done with one to four arguments, as shown in the syntax 
above. 
The status of the operation (i) is always returned as the value of the 
function ICHECK, so it can often be called with only one argument.  If it is 
desired to store the status in an indicator variable, ICHECK can be called 
with two arguments.  For example, the following statements have the same 
effect: 
    CALL ICHECK(3,J) 
      and 
    J = ICHECK(3) 
To obtain the count, ICHECK must be called with three arguments.  It then 
returns the count in n, and returns the status both as the function's value 
and by storing it into i.  If the status is 1 or 3, the count (n) is not 
meaningful. 
Thus, the BUFFIN, BUFFERIN, BUFFOUT, BUFFEROUT, and ICHECK calls can be 
illustrated as: 
    CALL BUFFIN(unit,mode,start,words) 
    CALL BUFFERIN(unit,mode,start,words,indicator[,count[,code]]) 
    CALL BUFFOUT (unit,mode,start,words) 
    CALL BUFFEROUT(unit,mode,start,words,indicator[,count[,code]]) 
    CALL ICHECK(unit[,indicator[,count[,code]]]) 
These operations always result in the processing of only one physical (as 
opposed to a logical) record.  Data is read into, or written from, consecutive 
words in memory with no regard to the type of the variable specified as the 
starting location of the buffer.  That is, the variable start merely specifies 
the starting location. 
Examples: 
The following statements illustrate an alternate way to use ICHECK when more 
detailed error and abnormal condition analysis is desired: 
       . 
       . 
       . 
    10 CALL ICHECK(5,IND,ICOUNT,ICODE) 
       IF (ICODE) 10,20,30 
    C  NORMAL PROCESSING - 
    20 . . . 
       . 
       . 
       . 
    C  ABNORMAL OR ERROR CONDITION - 
    30 GO TO (33,20,40,33) IND 
    C  ERROR HAS OCCURRED 
    33 . . . 
       . 
00002
00003
00001
An identifier is a symbolic name that is used to name either a variable, an 
array, or a subprogram. 
Although there are no restricted identifiers in FORTRAN-77, it is advisable 
for the sake of clarity not to use identifiers that correspond to FORTRAN 
statements.  Note that there are identifiers in FORTRAN-77 that are 
predefined. 
The type of an identifier in a program unit may be of any of the following 
data types: 
    integer 
    real 
    double precision 
    complex 
    double precision complex 
    logical 
    character 
    half-word integer (integer*2) 
    byte logical (logical*1) 
There is no identifier type corresponding to a literal datum.  An identifier 
of type character is used to represent a character datum. 
If an identifier has not been declared with a particular data type (see 
CLASSIFICATION_OF_IDENTIFIERS), the following implicit typing conventions are 
assumed: 
1.  Variables whose identifiers begin with the letters I, J, K, L, M, or N are 
    of type integer.  (This is referred to as the "IJKLMN rule".) 
2.  Variables whose identifiers begin with any other letter are of type real. 
Consequently, double precision, complex, double precision complex, logical, 
byte logical, half-word integer, and character identifiers must be explicitly 
declared as such (see EXPLICIT_TYPE_STATEMENTS). 
IF_SUBPROGRAM_COMPARE
It is often desirable to change the logical flow of a program on the basis of 
some test.  Arithmetic IF, logical IF and block IF THEN statements, which may 
be called conditional transfer statements, are used for this purpose.  The 
ELSE IF, ELSE, and END IF statements are used in conjunction with block IF 
THEN statements to control the logical flow of the program. 
See ARITHMETIC_IF, LOGICAL_IF, BLOCK_IF, ELSE_IF, ELSE, and END_IF. 
Syntax: 
IF (e)k1,k2,k3 
Parameters: 
e    is an expression of integer, real or double precision data type. 
ki    are statement labels; a comma may optionally precede k1. 
Description: 
The arithmetic IF statement is interpreted to mean 
    IF e<0, GO TO k1 
    IF e=0, GO TO k2 
    IF e>0, GO TO k3 
Note that if e is a real or a double precision expression, a test for exact 
zero may not be meaningful on a binary machine.  If the expression involves 
any amount of computation, a very small number is more likely to result than 
an exact zero.  For this reason, floating-point arithmetic IF statements 
should not usually be programmed to have a unique branch for zero. 
Examples: 
    Statement             Expression Value             Transfer to 
    IF (K)1,2,3                     47802                    3 
    IF (3*M(J)-7)76,4,3             -6                      76 
    IF (C(J, 10)+A/4)23,12,12       0.0002                  12 
    IF (IF(X, Y))704,17,90          1                       90 
    IF (K*N**2-14*LIMIT)78,444,78   -1000                   78 
    IF (Z-B-3.1416+SQRT(X-2))3,3,7  23.40669                 7 
00002
00003
00001
Syntax: 
IF (e) THEN 
Parameters: 
e    is an expression of logical type. 
Description: 
The block IF statement is used with the END IF statement and, optionally, the 
ELSE IF and ELSE statements to control the execution sequence flow. 
An IF-level and an IF-block are associated with block IF statement processing. 
The IF-level of a statement s is 
    m1 - m2 
m1   is the number of block IF statements from the beginning of the program 
unit up to and including s, and m2 is the number of END IF statements in the 
program unit up to but not including s.  The IF-level must be positive or 
zero.  The IF-level of the END statement of each program unit must be zero. 
The IF-level of each block IF, ELSE IF, and END IF statement must be positive. 
The IF-level of an ENTRY statement must be zero.  The IF-level of SUBROUTINE, 
FUNCTION, and PROGRAM statements is zero. 
Usage Notes: 
An IF-block consists of all of the executable statements that appear following 
the block IF statement up to, but not including, the next ELSE IF, ELSE, or 
END IF statement that has the same IF-level as the block IF statement.  An 
IF-block may be empty. 
Execution of a block IF statement causes evaluation of the expression e.  If 
the value of e is true, normal execution sequence continues with the first 
statement of the IF-block.  If the value of e is true and the IF-block is 
empty, control is transferred to the next END IF statement that has the same 
IF-level as the block IF statement.  If the value of e is false, control is 
transferred to the next ELSE IF, ELSE, or END IF statement that has the same 
IF-level as the block IF statement. 
Transfer of control into an IF-block from outside the IF-block is prohibited. 
If the execution of the last statement in the IF-block does not result in a 
transfer of control, control is transferred to the next END IF statement that 
has the same IF-level as the block IF statement that precedes the IF-block. 
The ELSE IF, ELSE, and END IF statements are used in conjunction with the 
block IF statements. 
00002
00001
00003
Syntax: 
ELSE IF (e) THEN 
Parameters: 
e    is an expression of logical type. 
ELSE IF Block 
An ELSE IF block consists of all of the executable statements that appear 
following the ELSE IF statement up to, but not including, the next ELSE IF, 
ELSE, or END IF statement that has the same IF-level as the ELSE IF statement. 
An ELSE IF block may be empty. 
Description (ELSE IF Statement): 
Execution of an ELSE IF statement causes evaluation of the expression e.  If 
the value of e is true, normal execution sequence continues with the first 
statement of the ELSE IF block.  If the value of e is true and the ELSE IF 
block is empty, control is transferred to the next END IF statement that has 
the same IF-level as the ELSE IF statement.  If the value of e is false, 
control is transferred to the next ELSE IF, ELSE, or END IF statement that has 
the same IF-level as the ELSE IF statement. 
Transfer of control into an ELSE IF block from outside the ELSE IF block is 
prohibited.  The statement label, if any, of the ELSE IF statement must not be 
referenced by any statement. 
If execution of the last statement in the ELSE IF block does not result in a 
transfer of control, control is transferred to the next END IF statement that 
has the same IF-level as the ELSE IF statement that precedes the ELSE IF 
block. 
00003
00002
00001
END_IF
Syntax: 
IF (e)[,]s 
Parameters: 
e    is an expression of logical type. 
s    is any executable statement other than a DO statement, a REPEAT 
statement, a block IF statement, ELSE IF, ELSE or END IF statement.  An END 
statement is not permitted. 
Description: 
For the logical IF, statement s is executed if the value of the expression e 
is true; if the value of the expression is false, the next executable 
statement following the logical IF statement is executed.  Whether the value 
of the expression is true or false, the statement following the logical IF is 
always executed unless statement s causes a transfer. 
Note that the entire construct "IF (e) s" is treated as a single statement 
except for function side effects.  Statement s may be a construct where a 
logical IF may control another IF, and so on.  In this case, the "false" exit 
for all of the logical IF statements is the same, namely, the statement 
following the whole construct.  For example, the statement 
    IF (A > B) IF (L1 .OR. L2)IF(SSWTCH(3)) GO TO 7 
is equivalent to: 
    IF ((A > B) .AND. (L1 .OR. L2) .AND. SSWTCH(3)) GO TO 7 
Statement s may also be a compound statement, all of which is controlled by 
the IF statement.  (See COMPOUND_STATEMENTS.) 
Examples: 
    IF (X .LE. 0) PAUSE 'X TOO SMALL';X=1.0;GO TO 14 
If X is less than or equal to zero, then the three statements 
    PAUSE 'X TOO SMALL' 
    X=1.0 
    GO TO 14 
are executed; otherwise, control passes to the next statement. 
Further examples: 
    IF (FLAG .OR. L) GO TO 3135 
    IF (W .OR. N<U/S+X3(J,K))B=R(J-8)=Q*ABS(X) 
    IF (OCTT*TRR .LT. 5.334E4),CALL THERMAL(N,Y(L,-5)) 
    IF (L4 .AND. .NOT. SLITET(9)) SWITCH=ON= .TRUE.;RETURN 
    IF (.NOT. SWITCH2) REWIND 
    IF (A<B)C=B-A;IF(D .LT. C)A=B;GO TO 10 
00002
00003
00001
IF
Syntax: 
IF (arg1[,arg2][,arg3]) 
Parameters: 
each arg    is an integer, real or double precision expression. 
Description: 
The purpose of this function is to facilitate running FORTRAN programs that 
were prepared on decimal-oriented or character-oriented machines (e.g., IBM 
1620, 1401, 7070, etc.).  On such machines, a test for equality between 
floating point values is often meaningful.  On a binary machine, however, most 
values are only close approximations to decimal numbers; therefore, values 
that might compare equal on a decimal machine may differ by a small amount on 
a binary machine. 
The IF function can be used to obtain an approximate comparison in an 
arithmetic IF statement or a relational expression. 
Examples: 
    IF(X-Y)3,7,8 
X and Y must be exactly equal to cause a branch to 7.  This statement can be 
rewritten as: 
    IF(IF(X,Y,EPS))3,7,8 
or as 
    IF(IF(X,Y))3,7,8 
In the first case (IF(IF(X,Y,EPS))3,7,8), X and Y are compared for relative 
equality within the range EPS, where EPS is any value chosen by the user.  In 
the second case, where no third argument is provided, a standard value is 
provided, as shown below. 
Since relative comparisons with zero are not meaningful, an absolute 
comparison instead of a relative one is made, if either arg1 or arg2 is zero. 
Calling the IF function with only one argument is equivalent to calling it 
with two arguments, the second of which is zero. 
When a relative comparison is to be made (i.e., when neither arg1 nor arg2 is 
zero), the IF function returns a value of zero if the following relation holds 
true: 
     |arg1 - arg2| 
  _______________________ 
                           < e 
 MIN(|arg1|,|arg2|) 
    where e = arg3, if specified. 
If only two arguments are specified: 
    e=2**(-50) (i.e., ~9x10**(-16)   if either argument is double precision. 
    e=2**(-18) (i.e., ~3x10**(-6)   if neither argument is double precision. 
If the above relation does not hold true, the function returns an integer 
value with the sign of (arg1 - arg2). 
When either argument is zero (or only one argument is provided), an absolute 
comparison is made and the function returns a value of zero if the following 
relation holds true: 
    |arg|< e 
where e is defined as above.  Otherwise, it returns an integer with the sign 
of (arg1 - arg2) or with the sign of the single argument. 
00002
00003
00001
IF_BLOCK
IMPLICIT_STATEMENT
    Unless declared to be in a particular category or to be of a particular 
    data type, the appearance of an identifier in an executable or DATA 
    statement causes it to be implicitly classified according to the following 
    set of rules: 
1.  When applicable, an identifier is of type integer if it begins with I, J, 
    K, L, M, or N.  If it begins with any other letter, it is of type real. 
    (Implicit type classification may be altered by use of the IMPLICIT 
    statement.) 
2.  An identifier that is called with a CALL statement is a subprogram. 
3.  An identifier that appears in an expression, followed by an argument list 
    enclosed in parentheses, is a function subprogram.  This does not apply to 
    declared arrays. 
4.  An identifier that appears to the left of an equals sign, followed by a 
    dummy list enclosed in parentheses, is a statement function definition if 
    it also complies with the rules given under STATEMENT_FUNCTION. 
    Otherwise, it is an error.  Again, this does not apply to declared arrays. 
5.  Any other appearance of an identifier in an executable statement, or DATA 
    statement, or statement function definition (i.e., other than followed by 
    a left parenthesis or in a CALL statement) causes it to be classified as a 
    variable. 
6.  Intrinsic functions have an inherent type associated with them.  Inherent 
    type is not equivalent to implicit type. 
7.  Generic function references have a type dependent on the data type of the 
    arguments. 
    Furthermore, an identifier that does not appear in any executable 
    statement, nor in any DATA statement, nor in any statement function 
    definition, but which does appear in a COMMON, EQUIVALENCE, or GLOBAL 
    statement, is implicitly classified as a variable. 
Syntax: 
IMPLICIT C1,C2,C3,...,Cn 
Parameters: 
each Ci    is a type specification of the form 
    type(c1,c2,c3,...cm) 
    where 
    type    is one of the following type declarations. 
        INTEGER 
        INTEGER*2 
        REAL 
        DOUBLE PRECISION 
        COMPLEX 
        DOUBLE COMPLEX 
        LOGICAL 
        LOGICAL*1 
        CHARACTER 
        CHARACTER*k 
        CHARACTER*(e) 
        where 
        k    is an unsigned integer constant. 
        e    is a limited integer expression containing only integer 
        constants. 
    cj    is a single alphabetic character or two such characters separated by 
    a dash (minus sign).  The second character must follow the first in 
    alphabetic sequence.  For example, 
        Z,A-G,M-N,S 
Description: 
The IMPLICIT statement causes each identifier that begins with the character 
specified, or whose initial character is in one of the ranges of characters 
specified, to be implicitly classified with the type specified.  An IMPLICIT 
declaration may override the normal (IJKLMN) rule of implicit type 
classification.  It, in turn, may be overridden by an explicit type 
declaration. 
Examples: 
    IMPLICIT COMPLEX(C),LOGICAL(T,F,L-M),INTEGER(H-J,W) 
causes the following implicit type conversions to be in force. 
1.  Identifiers beginning with C are complex. 
2.  Identifiers beginning with T, F, L, or M are logical. 
3.  Identifiers beginning with H, I, J, or W are integer.  Note that the I and 
    J are unnecessary here, because these are normally integer. 
4.  Identifiers beginning with K or N are integer (normal convention). 
5.  All other identifiers are real (normal convention). 
The statement: 
    IMPLICIT DOUBLE PRECISION (A-Z) 
causes the implicit type of all identifiers to be double precision. 
Because it affects only the implicit typing rules, an IMPLICIT statement has 
no effect on the types of intrinsic functions whose inherent type overrides 
the implicit type.  It does affect all other functions (see FUNCTION_TYPES 
INTRINSIC_FUNCTIONS). 
An implicit type declaration must not conflict with any other implicit type 
declaration.  For example, the statement 
    IMPLICIT REAL(A-Z),INTEGER(N) 
is illegal because N is declared to be both real and integer. 
00002
00003
00001
Syntax: 
INCLUDE fid[,LIST] 
Parameters: 
fid    is filename[{.account|.account.password|..password}] 
LIST    optionally specifies that the statement in the included file be listed 
on the LS and LO.  See COMP_OPTIONS for a description of the SR compiler 
option. 
A filename is a string of file characters whose maximum length is system 
dependent.  The file character set consists of 
o   All alphabetic characters 
o   All numeric characters 
o   Special characters:  $ # % & * - @ : 
Note that blanks are not allowed in the fid. 
The file may be a source file or a compressed file.  An INCLUDE file may be 
nested up to 10 levels. 
Description: 
The INCLUDE statement is used to request that additional source statement be 
placed in the program unit at the point of this statement.  If account has not 
been specified as part of the fid, the accounts of the SR option are searched 
for a match. 
The content of the specified file is placed in the user program at the point 
of the INCLUDE statement.  The END statement is not allowed in the file; if 
present, it is not included in the user program. 
00002
00001
INCLUDE
Syntax: 
INPUT [,]k  or  INPUT (u)[,]k 
Parameters: 
k    is an input list of variables.  No expressions or character strings may 
appear. 
u    is the unit number expression, which may be integer, real, or double 
precision (truncated to integer if necessary).  If no unit number is 
specified, 105 will be used (the standard read unit), unless the US compiler 
option is specified, in which case 5 is the default. 
Description: 
This is form 1 of the INPUT statement.  See also INPUT_NAMELIST. 
The INPUT statement processes free-form records, reading as many values as 
there are items in the list k.  These values may be in free form, and there 
may be any number of them on each input line.  Comma, semicolon, and 
end-of-physical record are considered terminators.  If no value appears ahead 
of a terminator, the following values are assumed:  zero for numeric 
quantities, .FALSE.  for logical quantities, and blanks for character 
quantities.  Each INPUT statement begins processing with a new physical 
record.  Except for null values and end-of-line, the READ statement with * 
option and INPUT statement are very similar.  For example, the statement 
    INPUT X,Y,I,K,A(3),B(I,K) 
might read just the following line: 
    23.705, 4738, 8, 20, 0 85E12 
or it might read these three lines: 
    23.705 
    4.738E3,    8,   20.0, 
    85000000000000 
In the latter case, note that the comma after the 20.0 indicates that there 
must be another value on the line, and since no value was present, zero is 
substituted.  This is also illustrated in the following line: 
    .23705E2,  4738.,  8,  20,  ,85.E12 
Constants should not have embedded blanks.  Except for that, almost any 
meaningful representation of a value is permissible.  The rules governing the 
forms of constants are: 
1.  For integer, real, and double precision variables, the constant may be any 
    of those types and will be scanned using a G.0 (widthless) format (see 
    NUMERIC_INPUT_STRINGS under the topic FORMATS).  Or it may be a literal 
    constant (surrounded by quotes) or a hexadecimal constant (written as 
    X'...'), or an octal constant (written as O'...'). 
2.  Complex values are written just like complex constants, i.e., two numeric 
    constants enclosed within parentheses. 
3.  For logical variables, the constant may be any string of characters 
    containing the letter T or F.  The field is terminated by a comma or 
    semicolon or nonleading blank.  Thus, all of the following forms are 
    accepted: 
        T   F   TRUE   FALSE   .TRUE.   .FALSE. 
4.  For character variables, the constant must be a character constant. 
When an unsubscripted array name appears in the input list, the entire array 
is input in storage order (columnwise).  As many values will be read as there 
are elements in the array.  Implied DO loops may also be used.  A comma should 
be used after the word INPUT if the first item begins with a left parenthesis 
and no unit number has been specified.  Otherwise, the parenthesis would 
signify that there is a unit number present. 
Examples: 
    INPUT, ((A(I,J),I=1,10),J=1,10),X,Y 
Here is another example of an INPUT statement with various types of values: 
    Program 
    ------- 
    CHARACTER * 4,K 
    COMPLEX C(2) 
    LOGICAL L1,L2 
    INPUT XRX,DEF,J,K,C,L1,L2 
    Data 
    ---- 
    930E6, X'B9F00000', 2, 'WXYZ' 
    (3.745, 1.02), (1.,-1.),T,F 
00002
00003
00001
Syntax: 
INPUT [(u)] 
Parameters: 
u    is the unit number.  When not specified, it is assumed to be 105 (the 
standard default) unless the US compiler option is specified, in which case 5 
is the default. 
Description: 
The INPUT statement written without a specified list of variables specifies 
self-identified input in conjunction with the program NAMELIST.  This form of 
the INPUT statement is designed to do self-identified input.  That is, the 
variables being named explicitly in the input list in the statement.  This 
enables the user to decide at run time which variables (if any) are to be 
input, and to select different input variables from run to run. 
This statement processes records of the form 
    r1;r2;r3;...;rn 
where the ri are each a form of replacement (similar to an assignment 
statement).  Either semicolons, as indicated, or commas may be used to 
separate the ri.  There may be any number of ri on a record.  Except within 
character constants, blanks are ignored. 
Each of the replacements ri may take one of the following forms: 
    v = c 
    a(s1,s2,s3,...,sn)=c 
    a=c1,c2,c3,...,cm 
In the first form, 
v    is a variable. 
c    is a constant of an appropriate type. 
The second form is used to input into an array element.  Here, 
a    is the identifier of an array. 
s1    is a subscript, which may be a signed or unsigned integer constant. 
c    is an appropriate constant. 
n    is the number of dimensions of the array a. 
The third form specifies input into an entire array, in the manner as 
described for explicit input/output lists (see SIMPLE_LISTS under the topic 
IO_LISTS).  In this case: 
a    is an array name. 
cj    is a constant of an appropriate type. 
m    is equal to the number of elements in the array. 
When an entire array is specified as above, the constants cj are assigned to 
successive array elements in the order in which the array is stored, e.g., 
columnwise.  Note that the number of constants must be less than or equal to 
the number of elements in the array.  Successive occurrences of the same 
constant can be represented in the form 
k* constant 
where k is a non-zero integer constant specifying the number of times the 
constant is to occur.  The constants may appear on separate input records, in 
which case the separating comma is optional.  For example, 
    ALPHA=1.7302,-67, 
          4E-5 
          .87,24.0983281640957 
    100000000000.;X=0; etc. 
This is the only case in which a single INPUT item (an ri) may overlap from 
one record to another. 
Variables that are to be processed by an INPUT statement must have been 
referenced by a NAMELIST declaration statement so that their names can be 
recognized (known) at run time.  Note that this can be done by using a 
NAMELIST statement with no identifier list after it, which causes all 
permissible variables and arrays in the program unit to be placed in the 
program NAMELIST. 
The permissible ways of expressing a constant that appears on an input record 
are the same as for the INPUT statement form 1. 
Note that if the items in an OUTPUT statement are restricted to variables, 
array elements (with constant subscripts), and arrays, the resultant output 
records can be processed by an INPUT statement. 
INPUT processing terminates when an asterisk character not contained in a 
character constant is found in the input string. 
Examples: 
    I=55E-2;A(1,1,3)=4,DBL=2 
    J=3746E02;LGL(12,-2)=.FALSE. 
    CPX=(7.32D-2,3);K=0,ARRAY=0, 
    0,14,3,71,* 
or 
    R(55,-2)=55.349384531062851907 
    J=9 
    BOOLE=T;G'FLAG=FORTRAN4 
    C=(2,7.08364724286E-03) 
    ARRAY=1.0 
          2.718281828459046 
          3.141592653589793 
          -3944483 
    *END* 
The *END* at the end of the second example is the form of end record 
that is generated by the OUTPUT statement; it enables the user to separate 
OUTPUT records into "files" that can later be processed by one or more 
INPUT statements.  For example, 
    OUTPUT(4)A,B,C,*,ARRAY,*,CPX,J(3),* 
generates records that can be processed by three separate INPUT statements. 
00002
00003
00001
INPUT_FORM1
INPUT_FORM2
IO_STATEMENTS
SIMPLIFIED_IO
LIST_DIRECTED_INPUT_RECORD
INPUT_FORM1
INPUT_FORM2
INPUT_FORM1
INPUT_FORM2
An INQUIRE statement may be used to inquire about attributes of a named file, 
or of the file connected to a particular unit.  There are two forms of the 
INQUIRE statement:  by file and by unit.  Except for the specifiers 
identifying them, the two forms are essentially identical; they both contain 
the same set of specifiers identifying variables or array elements in which 
values for the attributes are to be returned.  However, the values of some of 
these attributes have different meanings depending upon which form is used. 
See HELP (FORTRAN) INQUIRE_BY_FILE and HELP (FORTRAN) INQUIRE_BY_UNIT. 
Syntax: 
INQUIRE(FILE=fin[,ACCESS=ax][,ACCOUNT=an][,BLANK=blnk][,DIRECT=dir] 
   [,ERR=s][,ERRCODE=ec][,EXIST=ex][,FORM=fm][,FORMATTED=fmt][,IOSTAT=ios] 
   [,KEY=kv][,KEYED=kd][,KEYL=kl][,NAME=fn][,NAMED=nmd][,NEXTREC=nx] 
   [,NRECS=nr][,NUMBER=num][,OPENED=op][,RANDOM=rn][,RECL=rl] 
   [RECSIZE=ars][,SEQUENTIAL=seq][,UNFORMATTED=unf][,USAGE=use]) 
Parameters: 
FILE=fin[,ACCESS=ax]...    are parameters described in the following table 
under the topic INQUIRE_SPECIFIERS 
  Option                    Description 
  ACCOUNT=an 
                            an    is a character expression giving the name of 
                            the account that is to be searched for the file 
                            name. 
  EXIST=ex 
                            ex    is a logical variable or logical array 
                            element that will be set to true if a file by that 
                            name exists; otherwise, ex will be set to false. 
  FILE=fin 
                            fin    is a character expression giving the name 
                            of the file being inquired about.  The named file 
                            may or may not be connected to a unit.  If the 
                            value of fin is not of an acceptable form as a 
                            file name, an error condition exists. 
  OPENED=op 
                            op    is a logical variable or logical array 
                            element that will be set to true if the file is 
                            connected to a unit in this program; otherwise, op 
                            will be set to false. 
  USAGE=use 
                            use    is a character variable or character array 
                            element that will be set to 'INPUT' if the file 
                            exists and cannot be updated; to 'CREATE' if it 
                            does not exist; and to 'UPDATE' if the file exists 
                            and can be updated. 
Description: 
The following rules apply to use of the INQUIRE by File statement: 
1.  The specifiers ACCESS=ax, BLANK=blnk, ERRCODE=ec, FORM=fm, KEY=kv, 
    NEXTREC=nx, NUMBER=num, RECL=rl and RECSIZE=ars will be set only if the 
    following conditions are true: 
    a.  The value of FILE=fin is acceptable as a file name to the processor; 
        that is, the ERR=s return is not taken and ios is 0. 
    b.  A file with the specified name (FILE=fin) exists; that is, the 
        EXIST=ex status specifier has the value .TRUE.. 
    c.  The named file is opened to a unit; that is, the NAMED=nmd status 
        specifier has the value .TRUE. and the OPENED=op status specifier has 
        the value .TRUE.. 
2.  The specifiers DIRECT=dir, FORMATTED=fmt, KEYED=kd, RANDOM=rn, 
    SEQUENTIAL=seq and UNFORMATTED=unf require that the file exist and that 
    there be no error (condition 1a and 1b, above). 
3.  The INQUIRE statement may be executed before a file is opened, after it is 
    opened, or after it is closed.  It may be executed before, while, or after 
    a file is connected to a unit. 
4.  All definitions caused by the INQUIRE statement are those that are current 
    at the time the statement is executed. 
See also: HELP (FORTRAN) INQUIRE_SPECIFIERS. 
00002
00007
00003
00004
00005
00001
00006
Syntax: 
INQUIRE (UNIT=u[,ACCESS=ax][,ACCOUNT=an][,BLANK=blnk][,DIRECT=dir] 
   [,ERR=s][,ERRCODE=ec][,EXIST=ex][,FORM=fm][,FORMATTED=fmt][,IOSTAT=ios] 
   [,KEY=kv][,KEYED=kd][,KEYL=kl][,NAME=fn][,NAMED=nmd][,NEXTREC=nx] 
   [,NRECS=nr][,NUMBER=num][,OPENED=op][,RANDOM=rn][,RECL=rl] 
   [RECSIZE=ars][,SEQUENTIAL=seq][,UNFORMATTED=unf][,USAGE=use]) 
Parameters: 
UNIT=u[,ACCESS=ax]...    are parameters described in the following table under 
the topic INQUIRE_SPECIFIERS 
  Option                    Description 
  ACCOUNT=an 
                            an    is a character variable or character array 
                            element that will be set to the name of the 
                            account that contains the file associated with the 
                            unit number. 
  EXIST=ex 
                            ex    is a logical variable or logical array 
                            element that will be set to true if the specified 
                            logical unit exists and is known to the run time 
                            processor; otherwise, ex becomes will be set to 
                            false. 
  OPENED=op 
                            op    is a logical variable or logical array 
                            element that will be set to true if a file is 
                            connected to the unit; otherwise, op will be set 
                            to false. 
  UNIT=u 
                            u    is an external unit identifier.  It 
                            identifies a unit to which a file may or may not 
                            be connected.  If a file is connected to this 
                            unit, it is the file being inquired about. 
  USAGE=use 
                            use    is a character variable or character array 
                            element that will be set to the appropriate mode 
                            if a file is associated with the unit number: 
                            'INPUT' for Input mode 
                            'UPDATE' for Update mode 
                            'CREATE' for Create mode 
                            If no file is associated with the unit number, the 
                            variable is undefined. 
Description: 
The following rules apply to use of the INQUIRE by Unit statement: 
1.  The specifier variables or array elements ACCESS=ax, BLANK=blnk, 
    DIRECT=dir, ERRCODE=ec, FORM=fm, FORMATTED=fmt, KEY=kv, KEYED=kd, KEYL=kl, 
    NAME=fn, NAMED=nmd, NEXTREC=nx, NUMBER=num, RANDOM=rn, RECL=rl, 
    RECSIZE=ars, SEQUENTIAL=seq, and UNFORMATTED=unf will be set only if the 
    specified unit exists and is known to the processor and if a file is 
    connected to that unit; otherwise, they will be undefined. 
2.  The INQUIRE statement may be executed before a file is opened, after it is 
    opened, or after it is closed.  It may be executed before, while, or after 
    a file is connected to a unit. 
3.  All definitions caused by the INQUIRE statement are those that are current 
    at the time the statement is executed. 
00002
00007
00003
00004
00001
00005
00006
INQUIRE_SPECIFIERS
Inquiry specifiers containing the following entities may be used in either 
form of the INQUIRE statement.  These specifiers have meaning only when 
EXIST=ex, OPENED=op, and NAMED=nmd have the value .TRUE..  For descriptions of 
other parameters for the INQUIRE statements, enter 
HELP (FORTRAN) INQUIRE_BY_FILE PARAMETERS 
 or HELP (FORTRAN) INQUIRE_BY_UNIT PARAMETERS 
  Option                    Description 
  ACCESS=ax 
                            ax    is a character variable or character array 
                            element that will be set to 'DIRECT' if the file 
                            is a direct-access file, to 'SEQUENTIAL' if the 
                            file is a sequential file, to 'RANDOM' if the file 
                            is a random access file, to 'KEYED' if the file is 
                            a keyed access file, and to 'TERMINAL' if the unit 
                            is assigned to a specific device whose 
                            organization is 'TERMINAL'.  No minimum character 
                            length is specified.  (Note:  Access for the 
                            INQUIRE statement is not necessarily the same as 
                            for the CP-6 SET command.) 
  BLANK=blnk 
                            blnk    is a character variable or character array 
                            element that will be set to 'NULL' or 'ZERO', 
                            which denotes the default for the treatment of 
                            blanks encountered in FORMATTED input records. 
  DIRECT=dir 
                            dir    is a character variable or character array 
                            element that is assigned the value 'YES' if DIRECT 
                            is included in the set of allowed access methods 
                            for the file, 'NO' if DIRECT is not included in 
                            the set of allowed access methods for the file, 
                            and 'UNKNOWN' if the processor is unable to 
                            determine whether or not DIRECT is included in the 
                            set of allowable access methods for the file. 
  ERR=s 
                            s    is a statement label for the error specifier. 
                            If an error condition occurs during execution of 
                            an INQUIRE statement and no IOSTAT=ios specifier 
                            is present, all specified variable elements is set 
                            and control is transferred to the statement with 
                            the label s. 
  ERRCODE=ec 
                            ec    is an integer variable or integer array 
                            element that will be set to the error code of the 
                            last operation on this unit.  Error code values 
                            are stored as a form of integer constant. 
  FORM=fm 
                            fm    is a character variable or character array 
                            element that will be set to 'FORMATTED' if the 
                            file was opened as formatted.  It will be set to 
                            'UNFORMATTED' if the file was opened as 
                            unformatted. 
  FORMATTED=fmt 
                            fmt    is a character variable or character array 
                            element that is assigned the value 'YES' if 
                            FORMATTED is included in the set of allowed forms 
                            for the file, 'NO' if FORMATTED is not included in 
                            the set of allowed forms for the file, and 
                            'UNKNOWN' if the processor is unable to determine 
                            whether or not FORMATTED is included in the set of 
                            allowed forms for the file. 
  IOSTAT=ios 
                            ios    is an integer variable or integer array 
                            element that will be set to zero if no error 
                            occurs during execution of the INQUIRE statement. 
                            If an error occurs during this execution, the 
                            following occurs: 
                            1.  If an IOSTAT= specifier is present, it will be 
                            set to a non-zero value (see VALUES_OF_IOS) and 
                            the normal exit is taken. 
                            2.  If no IOSTAT= specifier is present and an ERR= 
                            specifier is present, the error exit is taken. 
                            3.  If no IOSTAT= specifier is present and no ERR= 
                            specifier is present, program execution is 
                            terminated. 
  KEY=kv 
                            kv    is a character variable or character array 
                            element that will be set to the key value (as a 
                            character string) of the last record that was 
                            processed.  If no record has been processed, the 
                            value is set to blanks.  If the file is a 
                            'SEQUENTIAL' file or a 'RANDOM' file or the key 
                            length (LEN(kv)) is not large enough, the value of 
                            kv is set to one blank followed by a string of 
                            asterisks that fill the specifier kv. 
  KEYED=kd 
                            kd    is a character variable or character array 
                            element that is assigned the value 'YES' if KEYED 
                            is included in the set of allowed access methods 
                            for the file, 'NO' if KEYED is not included in the 
                            set of allowed access methods for the file, and 
                            'UNKNOWN' if the processor is unable to determine 
                            whether or not KEYED is included in the set of 
                            allowable access methods for the file. 
  KEYL=kl 
                            kl    is an integer variable or array element that 
                            will be set to the length of the key of the last 
                            record that was processed.  The length that is 
                            returned is the byte length of the last record. 
  NAME=fn 
                            fn    is a character variable or character array 
                            element that will be set to the name of the file 
                            if the file has a name; otherwise, it will be 
                            undefined.  Note that if this specifier appears in 
                            an inquire-by-file statement, its value will be 
                            the same as the name given in the FILE=fin 
                            specifier. 
  NAMED=nmd 
                            nmd    is a logical variable or logical array 
                            element that will be set to true if the file has a 
                            name; otherwise, it will be set to false. 
  NEXTREC=nx 
                            nx    is an integer variable or integer array 
                            element that will be set to the value of the key 
                            of the last processed record plus one (edit 
                            increment .001).  If the last record key did not 
                            have a length of three, the value is set to zero. 
                            If no record has been processed, the value is set 
                            to one.  If the file is not 'DIRECT' or 'KEYED', 
                            the value returned will be -1. 
  NRECS=nr 
                            nr    is an integer variable or integer array 
                            element that will be set to the number of records 
                            in the file if the file is a RELATIVE file.  The 
                            value will be meaningless if the file is not a 
                            RELATIVE file. 
  NUMBER=num 
                            num    is an integer variable or integer array 
                            element that will be set to the number of the 
                            logical unit that is currently connected to the 
                            file.  If there is no unit connected to the file, 
                            num is set to -1.  Note that if this specifier 
                            appears in an inquire-by-unit statement, its value 
                            is that of the UNIT=u specifier. 
  RANDOM=rn 
                            rn    is a character variable or character array 
                            element that is assigned the value 'YES' if RANDOM 
                            is included in the set of allowed access methods 
                            for the file, 'NO' if RANDOM is not included in 
                            the set of allowed access methods for the file, 
                            and 'UNKNOWN' if the processor is unable to 
                            determine whether or not RANDOM is included in the 
                            set of allowable access methods for the file. 
  RECL=rl 
                            rl    is an integer variable or integer array 
                            element that will be set to the record length of 
                            the file as specified in the OPEN statement.  The 
                            length is measured in characters for formatted 
                            files and in storage units for unformatted files. 
                            If the file is opened without RECL= specified, it 
                            is set to -1. 
  RECSIZE=ars 
                            ars    is an integer variable or integer array 
                            element that will be set to the length of the last 
                            transmitted record (READ and WRITE).  Note that 
                            this length is in bytes (characters). 
  SEQUENTIAL=seq 
                            seq    is a character variable or character array 
                            element that is assigned the value 'YES' if 
                            SEQUENTIAL is included in the set of allowed 
                            access methods for the file, 'NO' if SEQUENTIAL is 
                            not included in the set of allowed access methods 
                            for the file, and 'UNKNOWN' if the processor is 
                            unable to determine whether or not SEQUENTIAL is 
                            included in the set of allowed access methods for 
                            the file. 
  UNFORMATTED=unf 
                            unf    is a character variable or character array 
                            element that is assigned the value 'YES' if 
                            UNFORMATTED is included in the set of allowed 
                            forms for the file, 'NO' if UNFORMATTED is not 
                            included in the set of allowed forms for the file, 
                            and 'UNKNOWN' if the processor is unable to 
                            determine whether or not UNFORMATTED is included 
                            in the set of allowed forms for the file. 
00001
00002
00003
00004
00005
00006
00007
00008
00009
00010
00011
00012
00013
00014
00015
00016
00017
00018
00019
00020
00021
INQUIRE
The sequential formatted WRITE and READ statements that specify a character 
entity for a unit number are interpreted as internal file I/O statements.  In 
an internal file I/O operation, instead of actual input/output, data 
conversion takes place between an input/output list and an internal buffer 
area.  This buffer area is specified as the unit number, and is a character 
variable or array.  Whereas an external record has a certain physical length, 
the length of a record in an internal file I/O operation is the length of the 
character unit.  When multiple records are specified by the FORMAT being used, 
the unit must be a character array name, and each array element corresponds to 
a record.  Note that list-directed internal file operations are not permitted. 
The READ statement causes the character string that is specified as the unit 
to be decoded, according to the specified FORMAT, and stored into the items in 
the list k. 
If the FORMAT statement requires more characters from a record than are 
specified by the length of the unit, the extra characters are assumed to be 
blanks; they are not obtained from the next record.  A new record is begun 
only when specifically requested by the FORMAT Statement (see LIST_INTERFACE 
under the topic FORMATS).  In order to process more than one record, the 
character unit must be an array name (without subscripts). 
Examples: 
     Scalar Variable 
     --------------- 
     CHARACTER FILE*30,CITY*20 
     FILE='1.E7,S.HENRIETTA' 
     READ(FILE,200)BRICK,CITY 
200  FORMAT(2G) 
will set BRICK=1.E7 
         CITY=S.HENRIETTA 
     Array 
     ----- 
     CHARACTER FILE3(3)*10 
     REAL   X(4) 
     FILE3(1)='2' 
     FILE3(2)='4.0' 
     FILE3(3)='.6E1' 
     READ(FILE3,'(F/F/F/F)',END=99)X 
will set X(1)=2 
         X(2)=4 
         X(3)=6 
leave X(4) undefined and transfer control to statement label 99. 
00001
The WRITE statement causes the list items to be converted to character 
strings, according to the FORMAT specified in the list, and to be placed in 
the character variable or array elements. 
If the number of characters generated by the FORMAT statement is greater than 
the specified size of the record, the extra characters are lost; they are not 
filled into the following record.  A new record is begun only when 
specifically requested by the FORMAT statement.  In order to process more than 
one record, the character unit must be an array name. 
If fewer characters are generated than are necessary to fill the record, it is 
filled out with trailing blanks.  In formatted WRITE operations, each record 
is filled with blanks before any characters are stored (generated) into it. 
Example: 
The following statements might be used to create a FORMAT stored for later use 
in the array M: 
    CHARACTER M*10 
    WRITE (M,3)12,5 
  3 FORMAT (2H(F,I,1H.,I,1H) 
FORMAT would occupy the character variable M and would appear as 
    (F12b.5b)b 
00001
INTRINSIC_STATEMENT
Enter HELP (FORTRAN) INTRINSICS function 
or 
HELP (FORTRAN) GENERICS function 
to see an individual intrinsic or generic function, or enter ?? to view all 
the functions. 
For a list of subtopic names, enter: 
HELP (FORTRAN) INTRINSICS_GENERICS TOPICS 
HELP on vector intrinsic functions and vector subroutines is available via 
separate HELP topics; enter: 
HELP (FORTRAN) VECTOR_INTRINSIC function 
or 
HELP (FORTRAN) VECTOR_SUBROUTINE subroutine 
The symbol ~ placed next to an intrinsic function name indicates extension of 
an ANS intrinsic function. 
  Int.       Gen.     No.      Result     Arg    Definition 
  Name~      Name     Args                Type   of Function 
 ABS         ABS      1      I,R,D,C,K I,R,D,C,K Absolute value.  For 
                                                 complex, see CABS. 
 {ACOS  }    ACOS     1        R,D,C,K  R,D,C,K  Arc cosine in radians. 
 {ARCOS~}                                        For complex, see CACOS. 
 ACOSH       ACOSH               R,D      R,D    Inverse hyperbolic 
                                                 cosine. 
 AIMAG                1           R       C      Imaginary part of 
                                                 argument expressed as a 
                                                 real value. 
 AINT        AINT     1        R,D,C,K  R,D,C,K  Integer part of 
                                                 argument (fractional 
                                                 part truncated). 
 ALOG        LOG      1           R       R      Natural logarithm 
                                                 (base e). 
 ALOG10      LOG10    1           R       R      Common logarithm 
                                                 (base 10). 
 {AMAX~}     MAX      N>2         R       R      Maximum value.  All 
 {AMAX1}                                         arguments are compared 
                                                 as real values. 
 AMAX0                N>2         R       I      Maximum value.  All 
                                                 arguments are compared 
                                                 as integer values, the 
                                                 result is real. 
 {AMIN~}     MIN      N>2         R       R      Minimum value.  All 
 {AMIN1}                                         arguments are compared 
                                                 as real values. 
 AMIN0                N>2         R       I      Minimum value,  All 
                                                 arguments are compared 
                                                 as integer values, the 
                                                 result is real. 
 AMOD        MOD      2           R       R      Arg1(mod arg2). 
                                                 Evaluated as 
                                                 arg1-arg2* 
                                                 AINT(arg1/arg2) 
                                                 i.e., the sign is the 
                                                 same as arg1. 
                                                 Function undefined if 
                                                 arg2 = 0. 
 ANINT       ANINT    1           R,D     R,D    Nearest integer. 
                                                 The type of the 
                                                 result is the type of 
                                                 the argument. 
 {ASIN }     ASIN     1        R,D,C,K  R,D,C,K  Arc sine in radians. 
 {ARSIN}                                         For complex, see CASIN. 
 ASINH       ASINH    1          R,D      R,D    Inverse hyperbolic 
                                                 sign. 
 {ATAN }     ATAN     1,2      R,D,C,K  R,D,C,K  Arctangent in radians. 
 {ATAN2}     ATAN2                               Arg1=ordinate (y), arg2= 
                                                 abscissa (x).  If arg2 is 
                                                 not present, assumed 1. 
                                                 Result (R) is arctangent 
                                                 of arg1/arg2 quadrant 
                                                 allocated in the range 
                                                 -(pi)<R<(pi). 
                                                 ATAN(0,)=0. 
                                                 For the generic names 
                                                 ATAN, ATAN2, either 
                                                 one or two arguments 
                                                 are acceptable with 
                                                 either ATAN or ATAN2. 
                                                 For complex, see CATAN. 
 ATANH       ATANH    1           R,D     R,D    Inverse hyperbolic 
                                                 tangent. 
 CABS        ABS      1           R       C      Complex absolute value 
                                                 (i.e., modulus). 
                                                 CABS(x+iy)= 
                                                 ((x**2)+(y**2))**(1/2). 
 CACOS~      ACOS     1           C       C      Complex arc cosine. 
                                                 CACOS(Z)=(-i)* 
                                                 CLOG(Z+CSQRT((Z**2)-1)) 
 CASIN~      ASIN     1           C       C      Complex arc sine. 
                                                 CASIN(Z)=(-i)* 
                                                 CLOG(i*Z+CSQRT(1-Z**2)) 
 CATAN~      ATAN     1           C       C      Complex arctangent. 
                                                 CATAN(Z)=u+iv= 
                                                 -i/2(CLOG(1+iZ)- 
                                                 CLOG(1-iZ)), 
                                                 allocated such that 
                                                 -(pi)<u<(pi). 
 CCOS        COS      1           C       C      Complex cosine. CCOS(Z)= 
                                                 (e**iZ)+(e**(-iZ))/2. 
 CCOSH~               1           C       C      Complex hyperbolic 
                                                 cosine. 
                                                 CCOSH(Z)= 
                                                 (e**Z)+(e**(-Z))/2. 
 CDABS~      ABS      1           D       K      Double complex absolute 
                                                 value (modulus).  See 
                                                 CABS. 
 CDACOS~     ACOS     1           K       K      Double complex arc 
                                                 cosine.  See CACOS. 
 CDASIN~     ASIN     1           K       K      Double complex arc sine. 
                                                 See CASIN. 
 CDATAN~     ATAN     1           K       K      Double complex arc 
                                                 tangent.  See CATAN. 
 CDBLE~               1           K       C      Converts complex to 
                                                 double complex. 
 CDCOS~      COS      1           K       K      Double complex cosine. 
                                                 See CCOS. 
 CDCOSH~     COSH     1           K       K      Double complex hyperbolic 
                                                 cosine.  See CCOSH. 
 CDEXP~      EXP      1           K       K      Double complex 
                                                 exponential.  See CEXP. 
 CDINT~      AINT     1           K       K      Double complex number 
                                                 formed by the integer 
                                                 values of the real and 
                                                 imaginary parts of arg. 
                                                 CDINT(x+iy)=DCMPLX* 
                                                 (DINT(x)+i*DINT(y)). 
 CDLOG~      LOG      1           K       K      Double complex natural 
                                                 logarithm (base e).  See 
                                                 CLOG. 
 CDSIN~      SIN      1           K       K      Double complex sine.  See 
                                                 CSIN. 
 CDSINH~     SINH     1           K       K      Double complex hyperbolic 
                                                 sine.  See CSINH. 
 CDSQRT~     SQRT     1           K       K      Double complex square 
                                                 root.  See CSQRT. 
 CDTAN~      TAN      1           K       K      Double complex tangent. 
                                                 See CTAN. 
 CDTANH~     TANH     1           K       K      Double complex hyperbolic 
                                                 tangent.  See CTANH. 
 CEXP        EXP      1           C       C      Complex exponential 
                                                 (e**arg). 
                                                 CEXP(x+iy)=EXP(x)* 
                                                 (COS(y)+i*SIN(y)). 
 CHAR                 1           S       I      Converts integer value to 
                                                 single character. 
 CINT        AINT     1           C       C      Complex number formed by 
                                                 integer values of the 
                                                 real and imaginary parts 
                                                 of arg. 
                                                 CINT(x+iy)= 
                                                 CMPLX(AINT(x)+ 
                                                 i*AINT(y)). 
 CLOG        LOG      1           C       C      Complex natural 
                                                 logarithm (base e) 
                                                 CLOG(Z)=CLOG(x+iy)= 
                                                 u+iv=ln|Z|+i*ATAN(y,x) 
                                                 allocated such that 
                                                 -(pi)<v<(pi). 
 CMPLX       CMPLX    2           C     I,R,D,C  Converts two noncomplex 
                                                 numbers to a complex 
                                                 number. 
                                                 CMPLX(x,y)=x+iy. 
 CONJG                1           C       C      Complex conjugate. 
                                                 CONJG(x+iy)=x-iy.  (Has 
                                                 no effect if arg not 
                                                 complex.) 
 COS         COS      1           R     R,D,C,K  Cosine of angle in 
                                                 radians.  For complex, 
                                                 see CCOS. 
 COSH                 1           R       R      Hyperbolic cosine.  For 
                                                 complex, see CCOSH. 
 CSIN        SIN      1           C       C      Complex sine.  CSIN(Z)= 
                                                 (e**(iZ))-(e**(-iZ))/(2i) 
 CSINH~      SINH     1           C       C      Complex hyperbolic 
                                                 sine.  CSINH(Z)= 
                                                 (e**Z)-(e**(-Z))/2 
 CSNGL~               1           C       K      Converts double precision 
                                                 complex to complex. 
 CSQRT       SQRT     1           C       C      Complex square root. 
                                                 CSQRT(Z)= 
                                                 u+iv=(e**(in Z))/2, 
                                                 allocated such that u>0. 
 CTAN~       TAN      1           C       C      Complex tangent. 
                                                 CTAN(Z)=CSIN(Z)/CCOS(Z)= 
                                                 (e1-e2)/i(e1+e2) 
                                                 where e1=e**(iZ), 
                                                 and   e2=e**(-iZ). 
 CTANH~      TANH     1           C       C      Complex hyperbolic 
                                                 tangent CTANH(Z)= 
                                                 CSINH(Z)/CCOSH(Z)= 
                                                 (e1-e2)/(e1+e2) 
                                                 where e1=e**(iZ), 
                                                 and   e2=e**(-iZ). 
 DABS        ABS      1           D       D      Double precision 
                                                 absolute value. 
 {DACOS~ }   ACOS     1           D       D      Double precision arc 
 {DARCOS~}                                       cosine in radians. 
 DACOSH               1           D       D      Double precision 
                                                 inverse hyperbolic 
                                                 cosine in radians. 
 {DASIN~ }   ASIN     1           D       D      Double precision arc 
 {DARSIN~}                                       sine in radians. 
 DASINH               1           D       D      Double precision 
                                                 inverse hyperbolic 
                                                 sine in radians. 
 {DATAN }    ATAN     1,2         D       D      Double precision 
 {DATAN2}    ATAN2                               arctangent in radians. 
                                                 See ATAN. 
 DATANH               1           D        D     Double precision 
                                                 inverse hyperbolic 
                                                 tangent in radians. 
 DBLE        DBLE     1           D     I,R,D,C  Argument converted to a 
                                                 value with double 
                                                 precision. 
 DCMPLX~              2           K       D      Converts two noncomplex 
                                                 numbers to a double 
                                                 precision complex 
                                                 number.  See CMPLX. 
 DCONJG~              1           K       K      Double complex 
                                                 conjugate.  See CONJG. 
 DCOS        COS      1           D       D      Double precision cosine 
                                                 of angle in radians. 
 DCOSH       COSH     1           D       D      Double precision 
                                                 hyperbolic cosine. 
 DDIM        DIM      2           D       D      Double precision 
                                                 positive difference. 
                                                 See DIM. 
 DEXP        EXP      1           D       D      Double precision 
                                                 exponential (e**arg). 
 DFLOAT               1           D       I      Argument converted to 
                                                 double precision. 
                                                 Same as DBLE, but 
                                                 generally used with 
                                                 integer arguments. 
 DIM         DIM      2          I,R,D   I,R,D   Positive difference. 
                                                 DIM(x,y)=x - min(x,y). 
 DIMAG~               1           D       K      Imaginary part of a 
                                                 double precision complex 
                                                 argument, expressed as 
                                                 a double precision value. 
 DINT        AINT     1           D       D      Integer part of the 
                                                 argument expressed as a 
                                                 double precision value. 
 DLOG        LOG      1           D       D      Double precision natural 
                                                 logarithm (base e). 
 DLOG10      LOG10    1           D       D      Double precision common 
                                                 logarithm (base 10). 
 {DMAX~ }    MAX      N>2         D       D      Double precision 
 {DMAX1~}                                        maximum value. 
 {DMIN~ }    MIN      N>2         D       D      Double precision 
 {DMIN1~}                                        minimum value. 
 DMOD        MOD      2           D       D      Double precision arg1 
                                                 (mod arg2).  See AMOD. 
 DNINT       ANINT    1           D       D      Nearest integer. 
 DREAL~               1           D       K      Real part of a double 
                                                 precision complex 
                                                 argument, expressed as 
                                                 double precision value. 
 DSIGN       SIGN     2           D       D      Double precision 
                                                 magnitude of arg1 
                                                 with sign of arg2.  If 
                                                 arg2 is zero, the sign 
                                                 is positive. 
 DSIN        SIN      1           D       D      Double precision sine 
                                                 of angle in radians. 
 DSINH       SINH     1           D       D      Double precision 
                                                 hyperbolic sine. 
 DSQRT       SQRT     1           D       D      Double precision 
                                                 square root (positive 
                                                 value). 
 DTAN        TAN      1           D       D      Double precision 
                                                 tangent. 
 DTANH       TANH     1           D       D      Double precision 
                                                 hyperbolic tangent. 
 EXP         EXP      1        R,D,C,K  R,D,C,K  Exponential (e**arg). 
                                                 For complex, see CEXP. 
 FLOAT                1           R       I      Argument converted to a 
                                                 real value. 
 IABS        ABS      1           I       I      Integer absolute value. 
 IAND~                N>2         I       I      Logical AND (extract). 
 ICHAR                1           I       S      Converts the single 
                                                 character into an 
                                                 integer value. 
 ICOMPL~              1           I       I      Logical NOT (1's 
                                                 complement).  Same as 
                                                 INOT. 
 IDIM        DIM      2           I       I      Integer positive 
                                                 difference.  IDIM(j,k)= 
                                                 j-MIN(j,k). 
 IDINT       INT      1           I       D      Argument converted to 
                                                 an integer value. 
 IDNINT      NINT     1           I       D      Nearest integer. 
 IEOR~                N>2         I       I      Logical EOR (exclusive 
                                                 OR). 
 IEXCLR~              N>2         I       I      Logical EOR (exclusive 
                                                 OR).  Same as IEOR. 
 IF~                  1,2,3       I     I,R,D    Compare for approximately 
                                                 equal or approximately 
                                                 zero.  Facilitates 
                                                 running FORTRAN 
                                                 programs that were 
                                                 prepared on 
                                                 decimal-oriented or 
                                                 character-oriented 
                                                 machines. 
 INDEX                2           I       S      Returns an integer value 
                                                 indicating the starting 
                                                 position within character 
                                                 string of arg1 where a 
                                                 substring identical to 
                                                 string arg2 is located. 
                                                 If arg2 is not found, 
                                                 INDEX returns a zero. 
 INOT~                1           I       I      Logical NOT (1's 
                                                 complement). 
 INT         INT      1           I     I,R,C    Argument converted to 
                                                 an integer value. 
 IFIX                 1           I       R      Argument converted 
                                                 to an integer value. 
 IOR~                 N>2         I       I      Logical OR (merge). 
 ISA~                 2           I       I      Integer Shift Arithmetic. 
                                                 Arg1 is shifted left 
                                                 arithmetically the number 
                                                 of bits specified in 
                                                 arg2.  If arg2 is 
                                                 negative, the shift is 
                                                 to the right.~ 
 ISC~                 2           I       I      Integer Shift Circular. 
                                                 Arg1 is shifted left 
                                                 circularly the number 
                                                 of bits specified in 
                                                 arg2.  If arg2 is 
                                                 negative, the shift 
                                                 is to the right.~ 
 ISIGN       SIGN     2           I       I      Integer magnitude of arg1 
                                                 with sign of arg2.  If 
                                                 arg2 is zero, the sign is 
                                                 positive.  Arg2 is not 
                                                 converted to integer. 
 ISL~                 2           I       I      Integer shift Logical. 
                                                 Arg1 is shifted left 
                                                 logically the number of 
                                                 bits specified in arg2. 
                                                 If arg2 is negative, the 
                                                 shift is to the right.~ 
 LEN                  1           I       S      Returns declared length 
                                                 of argument. 
 LGE                  2           L       S      Compares the left 
                                                 character argument with 
                                                 the righthand character 
                                                 argument to see if the 
                                                 left is greater than or 
                                                 equal.  If so, it 
                                                 returns an answer True; 
                                                 otherwise, an answer of 
                                                 False. 
 LGT                  2           L       S      Compares the left 
                                                 character. 
                                                 argument with the right 
                                                 character argument to see 
                                                 if the left is greater. 
                                                 If it is, it returns an 
                                                 answer of True; otherwise 
                                                 an answer of False. 
 LLE                  2           L       S      Compares the left 
                                                 character argument with 
                                                 the right character 
                                                 argument to see if the 
                                                 left is less than or 
                                                 equal.  If it is, it 
                                                 returns an answer of 
                                                 True; otherwise, an 
                                                 answer of False. 
 LLT                  2           L       S      Compares the left 
                                                 character argument to 
                                                 see if the left is less. 
                                                 If it is, it returns an 
                                                 answer of True; otherwise 
                                                 an answer of False. 
 LOCF~       NONE     1           I   All types  The LOCF intrinsic 
                                                 accepts an argument of 
                                                 any mode (one argument). 
                                                 The result, which is 
                                                 returned in the Q 
                                                 register (integer 
                                                 result), is a NSA pointer 
                                                 of the address of the 
                                                 argument. 
             LOG      1         R,D,C   R,D,C    See ALOG. 
             LOG10    1         R,D,C   R,D,C    See ALOG10. 
 MAX0        MAX      N>2         I       I      Integer maximum value. 
 MAX1                 N>2         I       R      Integer maximum value. 
                                                 All arguments are 
                                                 compared in real.  The 
                                                 result is integer. 
 MIN0        MIN      N>2         I       I      Integer minimum value. 
 MIN1                 N>2         I       R      Integer minimum value. 
                                                 All arguments are 
                                                 compared in real.  The 
                                                 result is integer. 
 MOD         MOD      2         I,R,D   I,R,D    Arg1 (mod arg2). 
                                                 Evaluated 
                                                 arg1-arg2* 
                                                 [arg1/arg2] 
                                                 where the brackets 
                                                 indicate integer part; 
                                                 i.e., the sign is the 
                                                 same as arg1.  Function 
                                                 is undefined if arg2=0. 
 NINT        NINT     1           I       R,D    Generic only.  Nearest 
                                                 integer. 
 REAL        REAL     1           R     I,R,C,D  Real part of a complex 
                                                 number. 
 SIGN        SIGN     2         I,R,D   I,R,D    Magnitude of arg1 with 
                                                 sign of arg2.  If arg2 
                                                 is zero, the sign is 
                                                 positive.  The type of 
                                                 arg2 must be real. 
 SIN         SIN      1        R,D,C,K  R,D,C,K  Sine of angle in radians. 
                                                 For complex, see CSIN. 
 SINH        SINH     1        R,D,C,K  R,D,C,K  Hyperbolic cosine.  For 
                                                 complex, see CSINH. 
 SNGL                 1           R       D      Argument converted to a 
                                                 value with real (single) 
                                                 precision. 
 SQRT        SQRT     1        R,D,C,K  R,D,C,K  Square root (positive 
                                                 value).  For complex, 
                                                 see CSQRT. 
 TAN         TAN      1        R,D,C,K  R,D,C,K  Tangent of angle in 
                                                 radians.  For complex, 
                                                 see CTAN. 
 TANH        TANH     1        R,D,C,K  R,D,C,K  Hyperbolic tangent.  For 
                                                 complex, see CTANH. 
00001
00002
00003
00004
00005
00006
00007
00008
00009
00008
00010
00011
00010
00012
00013
00002
00014
00014
00015
00016
00017
00018
00019
00020
00021
00022
00023
00024
00025
00026
00027
00028
00029
00030
00031
00032
00033
00034
00035
00036
00037
00038
00039
00040
00041
00042
00043
00044
00045
00046
00047
00048
00049
00050
00051
00052
00053
00054
00055
00054
00056
00056
00057
00058
00059
00060
00061
00062
00063
00064
00065
00066
00067
00068
00069
00070
00071
00072
00073
00074
00075
00076
00077
00078
00079
00080
00081
00082
00083
00084
00085
00086
00087
00088
00089
00090
00091
00092
00093
00094
00095
00099
00096
00097
00098
00100
00101
00102
00103
00104
00105
00106
00107
00108
00109
00110
00111
00112
00113
00114
00115
00116
00117
00118
00119
00120
00121
00122
00123
00124
00125
00126
Syntax: 
INTRINSIC f1,f2,...,fn 
Parameters: 
n>1 
each fi    must be a function name in the intrinsic function table. 
Description: 
The INTRINSIC statement is used to declare an intrinsic function name so that 
it may be passed as an argument. 
The purpose of this statement is to denote the intrinsic functions that are to 
be passed as actual arguments. 
If a function name, fi, is not in the intrinsic function table, the statement 
is in error and the function name fi will be treated as though it appeared in 
an EXTERNAL statement.  Note that in this error condition, implicit typing 
rules determine the type of the function. 
Generic function names may also appear in an INTRINSIC statement.  However, 
some of these functions may not be passed as arguments. 
00002
00001
IO_STATEMENTS
VALUES_OF_IOS
INTERNAL_FILE_IO
An input/output list defines the data that is to be processed by the 
input/output statement in which the list appears.  This list may be simple or 
contain implied DOs. 
A simple list has the form 
e1,e2,e3,...,en 
where 
each ei    may be a variable, an array element, an array name, an expression, 
an array block designator, or another input/output list enclosed in 
parentheses.  This implies that input/output lists may be nested to any level; 
furthermore, redundant parentheses are permissible.  However, parentheses are 
mandatory only when DO-implied lists are being nested (see below). 
Since it is not permissible to input into an expression (this includes a 
constant), expressions are normally reserved for output lists.  (See, however, 
ADJUSTABLE_FORMATS.) 
Examples: 
    Input Lists                   Output Lists 
    -----------                   ------------ 
    A                             E 
    B(5):B(10)                    C(I):C(5) 
    MATRIX(25,L)                  ALPHA(J,N) 
    MATRIX,T                      3*SQRT(X-Y),2.53 
    RY,Y(N,M),(X23A,HB,XKE)       A(J)+5,CSIN(C3)**(0,1), .TRUE. 
When an (unsubscripted) array name appears in an input/output list (sometimes 
called "short form"), it refers to all of the elements in the array in storage 
order.  (See ARRAY_STORAGE.)  This means that the elements are accessed 
beginning with the lowest subscript value (usually 1) for each dimension and 
ending with the maximum subscript value for each dimension.  In between, the 
first subscript varies most rapidly and the last subscript varies least 
rapidly.  (This is also called "columnwise" ordering.) 
For example, if V is 2 x 3 x 2 array, then the list item 
    V 
is equivalent to the following array elements in the order shown. 
    V(1,1,1) 
    V(2,1,1) 
    V(1,2,1) 
    V(2,2,1) 
    V(1,3,1) 
    V(2,3,1) 
    V(1,1,2) 
    V(2,1,2) 
    V(1,2,2) 
    V(2,2,2) 
    V(1,3,2) 
    V(2,3,2) 
00002
00001
Specifiers are used as part of the input/output statement to designate 
particular files or to qualify the function of the statement.  These 
specifiers include: 
    UNIT 
    FORMAT 
    RECORD 
    ERROR 
    IO_STATUS 
    END_OF_FILE 
To display a particular specifier (such as Unit), enter: 
HELP (FORTRAN) SPECIFIERS UNIT 
To receive a display of all specifiers, enter ??. 
UNIT 
A logical unit number is a means of referring to a file.  A unit has an 
attribute of being connected or not connected.  A unit may become connected by 
preconnection (via the IBEX SET command) or by the execution of a FORTRAN OPEN 
statement. 
Preconnection means that the logical unit is connected to a file at inception 
of execution of the executable program and therefore may be referenced by 
input/output statements without the prior execution of an OPEN statement. 
All input/output statements except OPEN, CLOSE and INQUIRE must reference a 
logical unit that is connected to a file. 
The form of a unit specifier is: 
[UNIT =] u 
where 
u    is an external unit identifier or an internal file identifier. 
An external unit identifier refers to an external file.  An internal file 
identifier refers to an internal file. 
An external unit identifier is one of the following: 
o   an integer expression i whose value must be zero or positive 
o   an asterisk, identifying a default unit specifier, 
    discussed under Default Unit Specifiers. 
(See DEFAULT_UNIT_SPECIFIERS.) 
The keyword form for the unit specifier is UNIT=.  (See KEYWORD_READ_WRITE.) 
See also IO_UNIT_ASSIGNMENTS 
FORMAT 
The form of a format specifier is: 
[FMT =] f 
where 
f    is a format identifier. 
A format specifier identifies a format, and may be any one of the following: 
1.  The statement label of a FORMAT statement in the same program unit as the 
    input/output statement. 
2.  The name of an integer variable to which has been assigned the statement 
    label of a FORMAT statement in the same program unit as the input/output 
    statement. 
3.  The name of an array, other than a character array, whose value is 
    Hollerith data representing a valid FORMAT. 
4.  The name of a character variable, character array element, character array 
    name, or character expression whose value represents a valid FORMAT. 
5.  An asterisk, specifying list-directed formatting. 
The keyword form for the format specifier is FMT=.  (See KEYWORD_READ_WRITE.) 
RECORD 
The form of the record specifier is: 
REC = n 
where 
n    is an integer expression whose value is positive.  It specifies the 
number of the record that is to be read or written in a file connected for 
direct access. 
IO_STATUS 
The form of the input/output status specifier is: 
IOSTAT = ios 
where 
ios    is an integer variable or integer array element.  (When an error 
condition is encountered, ios is set to a positive integer value as returned 
in the Monitor Services ALTRET frame. 
ERROR 
The form of the error specifier is: 
ERR = k1 
where 
k1    is a statement label. 
END_OF_FILE 
The form of an end-of-file specifier is: 
END = k2 
where 
k2    is a statement label. 
00006
00006
00005
00002
00004
00004
00003
00001
Input statements provide the means of transferring data from external media to 
internal storage or from an internal file to internal storage.  Output 
statements provide the means of transferring data from internal storage to 
external media or from internal storage to an internal file.  Additionally, 
some input/output statements may specify that editing of the data is to be 
performed. 
In addition to the statements that transfer data, there are auxiliary 
input/output statements to inquire about or describe the properties of the 
connection to the external medium, and file-positioning input/output 
statements to manipulate the external medium. 
FORTRAN-77 provides the following types of input/output statements: 
    Data Transfer         Auxiliary           File Positioning 
    Input/Output          Input/Output        Input/Output 
    Statements            Statements          Statements 
    ------------          ------------        ---------------- 
    READ                  OPEN                BACKSPACE 
    WRITE                 CLOSE               ENDFILE 
    PRINT                 INQUIRE             REWIND 
AUXILIARY_IO_STATEMENTS
LIST_DIRECTED_IO_STATEMENTS
All input/output statements specify a logical unit number, either explicitly 
via the unit specifier, or by implication.  The logical unit number is 
specified by a character identifier or an integer, real, or double precision 
expression, truncated to integer if necessary.  This logical unit number 
corresponds either to an internal file or to an actual physical device or 
file.  The correspondence (called connection) to an actual physical device or 
file is obtained in one of three ways: 
1.  Through the CP-6 SET command with the unit number assigned to a device at 
    run time. 
2.  Through automatic recognition that the number is one of the "standard" 
    unit assignments, referring to a particular device. 
3.  Through execution of an OPEN statement. 
For example, a PRINT statement, with no unit specified, selects the line 
printer (in batch mode) and requires no further run time unit assignment. 
This standard unit assignment, however, as well as all other standard 
assignments, can be overridden with a CP-6 control command assignment or an 
OPEN statement. 
The standard unit assignments also provide a set of logical unit numbers that 
can be used with these FORTRAN-77 input/output statements that do not select a 
specific device to provide the same capability as above; that is, they provide 
for selection of a particular device that can be changed, if necessary, at run 
time.  For example, 
    READ(105,format) list   selects the card reader (in batch) or the 
                            user's terminal (on line). 
    WRITE(108,format) list  selects the line printer (in batch) or the 
                            user's terminal (on line). 
Keyword forms are alternate forms of the READ and WRITE statement. 
The keyword form for unit number is UNIT=u, where u is the logical unit 
number, and FMT=f for the format specifier where f is the format specifier. 
If both the unit number and format specifier appear in an input/output 
statement and the keyword form UNIT=u is used, the keyword form FMT=f must 
also be used.  In this case, the unit number and format specifier may appear 
in any order among the other specifiers (e.g., the ERR= and, if present, the 
END=, REC= or the KEY=). 
Examples: 
Examples of use of keyword forms: 
     Formatted Sequential READs 
     -------------------------- 
           READ(UNIT=10,FMT=20,ERR=99,END=90,IOSTAT=INT)A,B 
           READ(ERR=901,UNIT=7,END=70,FMT=921)Z1,Z2 
           READ(FMT=21,END=999,ERR=707,UNIT=I)A,B,C,D 
     Formatted Direct-Access READs 
     ----------------------------- 
           READ(UNIT=9,FMT=12,REC=J,IOSTAT=I(J),ERR=99)X,Y,Z,A 
           READ(REC=J*1000,ERR=99,FMT='(I,NA)',UNIT=L)I,I,C 
           READ(FMT=ARRAY,UNIT=U1,REC=IU1)J,K,L,M,N 
     Formatted Keyed-Access READs 
     ---------------------------- 
           READ(UNIT=21,FMT=101,KEY=1000)A(K),A(K+1) 
           READ(KEY='ABCDE',UNIT=A,FMT=B)W1,W2,W3 
           READ(ERR=11,KEY=I*1000+J,UNIT=1,FMT=7)U1,U2 
     Formatted Sequential WRITEs 
     --------------------------- 
           WRITE(UNIT=10,FMT=L,ERR=21)'POSITION',A,B,X 
           WRITE(UNIT=11,ERR=9,IOSTAT=IJK,FMT=27)A+B,C(I),J*K 
           WRITE(FMT=901,ERR=77,UNIT=11)A*B,C,D/E,F 
     Formatted Direct-Access WRITEs 
     ------------------------------ 
           WRITE(UNIT=11,FMT=27,REC=91,ERR=92)A,B,C 
           WRITE(FMT=21,REC=9,UNIT=11)Z1,Z11,Z2,Z21 
           WRITE(REC=11,FMT=17,ERR=101,UNIT=U)A1,A2,A3 
     Formatted Keyed-Access WRITEs 
     ----------------------------- 
           WRITE(UNIT=11,FMT=71,KEY=J)K,L,M 
           WRITE(KEY=21000,UNIT=A,FMT=B)ALPHA,BETA,Z1 
           WRITE(ERR=101,KEY=21500,UNIT=A,FMT=B)GAMMA,DELTA 
     Unformatted Sequential READs 
     ----------------------------- 
           READ(UNIT=1,END=2,ERR=1)A,X1,OM,HIGH 
           READ(END=7,IOSTAT=J,UNIT=1,ERR=1)Z,Z2,X1,X(I) 
           READ(ERR=11,END=7,UNIT=1)W,LAMBDA,ZETA 
     Unformatted Direct-Access READs 
     ------------------------------- 
           READ(UNIT=2,REC=7,ERR=11)W,X 
           READ(REC=8,UNIT=2,ERR=12)Y,Z 
     Unformatted Keyed-Access READs 
     ------------------------------ 
           READ(UNIT=4,KEY=2000,ERR=12)X,Y 
           READ(KEY='XYZ',UNIT=5,ERR=11)X1,X2,Z 
     Unformatted Sequential WRITEs 
     ----------------------------- 
           WRITE(UNIT=3,ERR=12)SIN(X),X,Y 
           WRITE(UNIT=I,ERR=21)Z,Z1,W 
     Unformatted Direct-Access WRITEs 
     -------------------------------- 
           WRITE(UNIT=U1,REC=IRECORD)Z(1),Y(1) 
           WRITE(REC=JRECORD,UNIT=12,ERR=7)X,Y,Z 
     Unformatted Keyed-Access WRITEs 
     ------------------------------- 
           WRITE(UNIT=U1,KEY=C//'ABC')Z(1),X(1) 
           WRITE(KEY=I,UNIT=3,ERR=11)A,B,C 
     List Directed Sequential READs 
     ------------------------------ 
           READ(UNIT=7.FMT=*,ERR=71)A,B,C,D,E 
           READ(FMT=*,UNIT=71,END=747)X,Y,Y2,Z1 
     List Directed Sequential WRITEs 
     ------------------------------- 
           WRITE(UNIT=11,FMT=*,ERR=21,IOSTAT=IK)ALP,MONT,BLANC 
           WRITE(FMT=*,UNIT=X,ERR=23)X,Y,Z 
00001
READ_KEYWORD
WRITE_KEYWORD
LABELS
Labeled COMMON blocks are discrete sections of the COMMON region and as such 
are independent of each other and blank COMMON. 
Any labeled COMMON block may be referenced by any number of programs or 
subprograms that comprise an executable program.  References are made by block 
name, which must be identical in all references.  Only those labeled COMMON 
blocks that contain data needed by a given program require definition; other 
blocks need not be defined. 
The variables defined as being in a particular labeled COMMON block do not 
necessarily have to correspond in type or number between the programs in which 
the block is referenced.  However, the size definition of a labeled COMMON 
block should be less than or equal to the size defined in the first program in 
which it appears. 
Examples: 
    SUBROUTINE A            SUBROUTINE B 
    REAL T,V,W,X(0:20)      COMPLEX G,F(0:10) 
    COMMON /SET1/T,V,W,X    COMMON /SET1/G,F 
    .                       . 
    .                       . 
    .                       . 
Both references to the COMMON block SET1 correspond in size.  That is, both 
subprograms define the block SET1 as containing 24 words:  24 items of real 
type in subroutine A, 12 items of complex type in subroutine B. 
Reference may be made to the name of a labeled COMMON block more than once in 
any program.  Multiple references may occur in a single COMMON statement, or 
the block name may be specified in any number of individual COMMON statements. 
In both cases, the processor links all variables defined as being in the block 
into a single labeled COMMON block of the appropriate name. 
Block names must be unique with respect to 
1.  Subprogram names defined explicitly or implicitly to be external 
    references (see EXTERNAL). 
2.  Other block names. 
3.  GLOBAL variables. 
A labeled COMMON block may have the same name as an identifier in any 
classification other than the above; however, it is usually preferable to 
choose block names that are unique. 
00001
Each executable statement in a FORTRAN-77 program is executed in the order of 
its appearance in the source program, unless this sequence is interrupted or 
modified by a control statement that transfers program control to another 
statement.  Labels are used to identify statements.  A nonexecutable statement 
may have a label, but the label of a nonexecutable statement should not be 
referenced except in the case of a FORMAT statement. 
Statement labels consist of up to five decimal digits and must be greater than 
zero; embedded blanks and leading zeros are not significant.  The following 
labels are equivalent: 
    857      8 5 7 
    00857    085 7 
ADDITIONAL_LIBRARY_SUBPROGRAMS
FORTRAN-77 provides a large number of library subprograms.  The intrinsic 
functions are specially recognized by the compiler, which generates special 
machine code for them.  Others exist as closed routines in the system library. 
Most of the library subprograms are function subprograms, but several 
subroutine subprograms are also provided. 
There are three kinds of function references:  standard, intrinsic, and 
generic.  Intrinsic function names and generic function names are recognized 
by the compiler whenever they appear in a program.  These function subprograms 
evaluate commonly used mathematical functions and can be handled more 
effectively by being specially treated. 
For additional information on this topics, enter: 
HELP (FORTRAN) FUNCTION_TYPES subtopic 
where subtopic can be any of the following:  STANDARD_FUNCTIONS, 
INTRINSIC_FUNCTIONS, or GENERIC_FUNCTIONS. 
Standard functions (those not intrinsic or generic) have the following 
characteristics: 
1.  A standard function is called with a standard calling sequence.  This 
    method of passing arguments, which is common to all FORTRAN subprograms, 
    contains the flexibility to handle all the various kinds of arguments: 
    arrays, expressions, statement numbers, subprogram names, etc.  Since this 
    method is standard, any standard function call can be satisfied by any 
    form of FORTRAN subprogram.  For example, a reference to the function 
    ALPHA, such as 
        X = ALPHA(Y) 
    can be satisfied by a FUNCTION subprogram, a statement function, an ENTRY 
    statement, or a PL/6 procedure. 
2.  A standard function is typed according to the usual rules.  For example, 
    assuming there are no other implicit or explicit type declarations, the 
    function ALPHA will be a real function since it does not begin with any of 
    the letters I, J, K, L, M, or N. 
Intrinsic functions have neither of the above characteristics. 
An intrinsic function name is known to the compiler and processed differently 
from an ordinary function.  It is not called with a standard calling sequence. 
Since intrinsic functions deal almost exclusively with arguments that are the 
values of expressions, they do not need the flexibility associated with 
handling other kinds of arguments, such as literal constants or statement 
numbers, and can use a more efficient means of passing their arguments. 
Sometimes, in fact, they pass no arguments at all.  For simple functions, such 
as absolute value, all of the instructions required to evaluate the function 
are generated in-line each time the function is referenced.  For more 
complicated functions, a special library routine is called.  This routine does 
not expect a standard calling sequence.  Furthermore, it does not have an 
ordinary identifier for a name but a special name that cannot conflict with 
any FORTRAN identifier.  For example, an intrinsic reference to the function 
SQRT might produce a call on the special routine XPE_SQRT. 
The effect of this is that an intrinsic function reference cannot be satisfied 
by any subprogram written in FORTRAN.  An intrinsic function is sometimes 
called an "open" subprogram, as opposed to a "closed" subprogram that can be 
called with a standard calling sequence.  An intrinsic function name can be 
passed as an argument if it is declared in an INTRINSIC statement.  This 
declaration does not declare the intrinsic function external. 
The second feature that distinguishes an intrinsic function from other 
functions is its inherent type.  Every intrinsic function reference has an 
inherent type that is independent of implicit typing rules and depends only on 
the function used.  The reason why intrinsic functions have inherent type is 
that it would be inconvenient to have to declare the type of every complex or 
double precision library function referenced in a program.  Using inherent 
type, the compiler is able to know, for example, that CSIN is complex and DLOG 
is double precision.  However, inherent type does not apply to an intrinsic 
function name that has been declared EXTERNAL, since the function is then no 
longer intrinsic. 
Rules: 
The identifier of an intrinsic function can lose its intrinsic characteristics 
in several ways: 
1.  By being declared EXTERNAL.  An intrinsic function declared EXTERNAL 
    becomes an external function. 
2.  By appearing in a dummy list. 
3.  By being defined elsewhere in the same program; i.e., in a FUNCTION, 
    SUBROUTINE, or ENTRY statement or as a statement function. 
4.  By appearing in a context other than as a function reference; for example, 
    in a DIMENSION statement, or as a variable. 
5.  By being CALLed. 
The IMPLICIT statement does not affect intrinsic or generic functions. 
A generic function name provides an alternate method of referencing certain 
processor-defined evaluations.  Most such evaluations may also be referenced 
by intrinsic function names. 
If a generic function name appears in an EXTERNAL statement or in an explicit 
type statement, its generic property is removed in that program unit. 
00003
00002
00001
IO_LISTS
DATA_CONSTANT_LIST
DATA_VARIABLE_LIST
A list-directed input record consists of a sequence of values and value 
separators.  Values are separated by one of the following four kinds of value 
separators: 
1.  One or more blanks 
2.  A comma optionally preceded by one or more blanks and optionally followed 
    by one or more blanks. 
3.  A slash optionally preceded by one or more blanks and optionally followed 
    by one or more blanks. 
4.  The end of a line, except within a character constant or complex constant. 
Each value is either a constant, a null value, or one of the forms 
r*c 
r* 
where r is an unsigned, non-zero, integer constant.  The r*c form is 
equivalent to r successive appearances of the constant c, and the r* form is 
equivalent to r successive null values.  Neither of these two forms may 
contain embedded blanks, except where permitted within the constant c. 
Neither may contain an embedded end of a line. 
Hollerith constants are not permitted in list-directed input records. 
Otherwise, any form acceptable in a formatted input field for a given type is 
also acceptable as a list-directed input value.  In addition, an integer 
constant is acceptable as a real constant, with an implied decimal point 
appended after the rightmost digit.  The form of the input value must be 
acceptable for the type of the list item.  Blanks are never used as zeros, and 
embedded blanks are not permitted in constants except within character 
constants and surrounding the comma between the real and imaginary parts of a 
complex constant.  See INPUT_STATEMENT for exceptions to the rule above. 
A null value is specified by any of the following:  (1) the r* form, (2) the 
first nonblank character being a slash or a comma, or (3) two commas separated 
by zero, one, or more blanks.  A null value causes the corresponding list item 
to be skipped.  The value of the list item is not changed.  A null value may 
not be used as either the real or imaginary part of a complex constant, but it 
may be supplied in place of an entire complex constant.  Note that the end of 
a line adjacent to any other separator, with or without separating blanks, 
does not generate a null line. 
A slash encountered in the input record causes termination of execution of the 
input statement after the assignment of any previous values.  If there are 
additional items in the input list, the effect is as though null values had 
been supplied for them. 
Note that a complex constant must have the form of a pair of real or double 
precision or integer constants enclosed in parentheses.  Embedded blanks are 
permitted immediately before or after the comma that separates the real part 
and the imaginary part, and the end of a line may occur between the real part 
and the comma or between the comma and the imaginary part. 
Character constants enclosed in apostrophes are permitted when the 
corresponding list item is of type character.  Each apostrophe within a 
character constant must be represented by two consecutive apostrophes with 
neither blanks nor the end of a line intervening.  Character constants may be 
continued from the end of one line to the beginning of the next line.  The end 
of the line does not cause a blank or any other character to become part of 
the constant.  The constant may be continued on as many lines as needed. 
Blanks, commas, and slashes may appear in character constants. 
Syntax: 
READ(u,*)[k] 
READ *,[k] 
and 
WRITE(u,*)[k] 
PRINT *,[k] 
Parameters: 
u    is a logical unit number.  It may be an integer, real, or double 
precision expression, truncated to integer if necessary. 
*    specifies that the operation is list-directed. 
k    is an I/O optional list.  If omitted, the I/O operation will produce one 
blank record on output and skip one record on input. 
Description: 
These statements process formatted records.  The format is implied by the data 
type of each item in the I/O list.  (For input formats, see INPUT_WITH_LIST.) 
The simplest forms are those shown above. 
00002
00001
A list-directed output record consists of a sequence of values that have the 
same form as constants, except as noted otherwise.  The only separators 
produced are blanks and the end of the line.  The processor may start new 
lines as necessary but, except for complex constants and character constants, 
the end of a line must not occur within a constant, and blanks must not appear 
within a constant. 
Logical output constants are T for true value and F for a false value. 
Real and double precision constants are produced with either the equivalent of 
a F field descriptor or an E field descriptor, depending on the value. 
Complex constants are enclosed in parentheses, with a comma separating the 
real and imaginary part. 
Hollerith constants are not produced in list-directed output records.  The end 
of a line will not occur within a character constant unless the constant is 
longer than an entire line. 
Character constants produced are not delimited by apostrophes, are not 
preceded or followed by a value separator, have each internal apostrophe 
represented by one apostrophe, and have a blank character inserted by the 
processor for carriage control at the beginning of any line that begins with 
the continuation of a character constant from the previous line. 
Note that list-directed input records produced by PRINT statements are not 
always acceptable as list-directed input records, but list-directed records 
produced by WRITE statements are always acceptable as list-directed input 
records. 
Commas and slashes are separators and null values (including the form r*) are 
not produced in list-directed output fields. 
Except for character constants continued from the previous line by a WRITE 
statement, each list-directed output line begins with a blank character to 
provide carriage control when the line is printed. 
DO_IMPLIED_LISTS
IO_LISTS
Logical expressions have the form 
e1 c1 e2 c2 e3 c3...en 
where 
ei    are logical elements, and 
ci    are the binary logical operators. 
When evaluated, logical expressions result in a value of either "true" or 
"false". 
A logical element is defined as one of the following entities: 
1.  A logical variable or function reference. 
2.  A logical constant. 
3.  A relational expression. 
4.  Any of the above enclosed in parentheses. 
5.  A logical expression enclosed in parentheses. 
6.  Any of the above, preceded by the unary logical operator .NOT. 
IF_LOGICAL
    There are six logical operators: 
    Operator                Type 
    --------                ---- 
    .NOT.                   Unary 
    .AND.                   Binary 
    .OR.                    Binary 
    .EOR. (exclusive OR)    Binary 
    .EQV. (equivalent)      Binary 
    .NEQV. (not equivalent) Binary 
                    | 
                    |                Evaluation 
                    |--------------------------------------------------- 
                    |           e1      e1      e1      e1       e1 
  Expression Values |    .NOT.  .AND.   .OR.    .EOR.   .EQV.    .NEQV. 
                    | e e2 e2 e2 e2 e2 
--------------------|--------------------------------------------------- 
                    | 
  e True            |    False    -       -       -      -        - 
                    | 
  e False           |    True     -       -       -      -        - 
                    | 
  e1 False e2 False |     -     False   False   False   True    False 
                    | 
  e1 True  e2 False |     -     False   True    True    False   True 
                    | 
  e1 False e2 True  |     -     False   True    True    False   True 
                    | 
  e1 True  e2 True  |     -     True    True    False   True    False 
                    | 
In summary: 
1.  .NOT. e is "true" only when e is "false". 
2.  e1 .AND. e2 is "true" only when both e1 and e2 are "true". 
3.  e1 .OR. e2 is "true" when either or both e1 and e2 are "true". 
4.  e1 .EOR. e2 is "true" when either but not both e1 or e2 is "true". 
5.  e1 .EQV. e2 is "true" when e1 and e2 are either both "true" or both 
    "false". 
6.  e1 .NEQV. e2 is "true" when either but not both e1 or e2 is "true". 
    As in arithmetic expressions, parentheses are used to define evaluation 
    sequences explicitly.  Consequently, 
    A .AND. B .OR. Q(3) .NE. X 
does not have the same meaning as 
    A .AND. (B .OR. Q(3) .NE. X) 
where (B .OR. Q(3) .NE. X) may be called a logical subexpression. 
The evaluation hierarchy for logical expressions is 
1.  Arithmetic expressions. 
2.  Relational expressions (the relational operators are all of equal 
    precedence). 
3.  The innermost logical subexpression, followed by the next innermost 
    logical subexpression, etc. 
4.  The logical operations, in the following order of precedence: 
    Operator     Order 
    --------     ----- 
    .NOT.       1 (highest) 
    .AND.       2 
    .OR.        3 
    .EOR.       4 
    .EQV.       4 
    .NEQV.      4 
For example, the expression 
    L .OR. .NOT. M .AND. X .GE. Y 
is interpreted as 
    L .OR. ((.NOT. M) .AND. (X .GE. Y)) 
Note that it is permissible to have two contiguous logical operators only when 
the second operator is .NOT.; in other words 
    e1 .AND. .OR. e2 
is not valid, while 
    e1 .AND. .NOT. e2 
is legal.  Two consecutive .NOT. operators are not permissible. 
00001
A main program consists of a set of FORTRAN-77 statements, the first of which 
(other than comment lines) is not one of the following statements: 
    a FUNCTION statement 
    a SUBROUTINE statement 
    a BLOCK DATA statement 
and the last is an END statement.  The first statement of a main program may 
be a PROGRAM statement. 
Once an executable program has been loaded, execution of the program begins 
with the first executable statement in the main program. 
The following statements are not permitted in a main program: 
    BLOCK DATA 
    ENTRY 
    FUNCTION 
    SUBROUTINE 
                            Data Type of the Expression Value 
Data type of 
the variable                                     Double 
or the array                 Double              Precision         Char- 
element       Integer  Real  Precision  Complex  Complex  Logical  acter 
Integer          X       I       I         I        I        N       N 
Real             F       X       P         Z        P        N       N 
Double           F       P       X         P        Z        N       N 
Precision 
Complex          F       R       R         X        P        N       N 
Double           F       D       D         P        X        N       N 
Precision 
Complex 
Logical          N       N       N         N        N        X       N 
Character        N       N       N         N        N        N       Y 
The symbols used in the table above have the following meanings: 
Symbol     Meaning 
------     ------- 
X       Direct assignment of the exact value. 
I       The value is truncated to integer.  The truncated value is equal to 
        the sign of the expression times the greatest integer less than or 
        equal to the absolute value of the expression (e.g., 4274.9983 is 
        truncated to 4274, and -0.6 to 0).  Values that are greater than the 
        maximum size of an integer will be truncated at the high-order end as 
        well.  Results in this case generally are not meaningful. 
F       The variable is assigned the real or double precision approximation of 
        the value.  Note that since real precision is less than that of 
        integers, conversion to real may cause a loss of significant digits. 
P       The precision of the value is increased or decreased accordingly. 
R       The real part of the variable is assigned the real approximation of 
        the expression.  The imaginary part of the variable is set to zero. 
D       The real part of the variable is assigned the double precision 
        approximation of the expression.  The imaginary part of the variable 
        is set to zero. 
N       Not allowed. 
Y       The expression is either extended on the right with blanks or 
        truncated from the right depending on the relative lengths of the 
        expression and the character variable. 
Z       Direct assignment of the real part of the value. 
Examples: 
    A=B 
    Q(I)=Z**2+N*(L - J) 
    L=F .OR. .NOT. C .AND. R .GE. 23.9238E-1 
    CRE (8, ED)=(ALL,MEN) 
    T32(0)=L/(34.,-0.113244)*(.0045,0) 
    PI=4*(ATAN(0.5)+ATAN(0.2)+ATAN(0.125)) 
    C(3:9)='ABCD'//'DCB' 
00001
An arithmetic expression that contains elements of more than one data type is 
termed a mixed expression.  Integer, real, double precision, complex, and 
double precision complex elements may be mixed in an arithmetic expression 
using any of the arithmetic operators (including **).  Logical elements may 
not appear in an arithmetic expression except as function arguments (see rule 
2 below). 
The order of type precedence is 
    Type                             Type Precedence 
    ----                             --------------- 
    Double precision complex         1 (highest) 
    Complex or double precision      2 
    Real                             3 
    Integer                          4 
In a mixed expression, the order of evaluation is more restrictive, because of 
integer division, than it is for expressions in which all of the elements are 
of the same data type.  The result of a unary operator operating on an element 
will be the same data type as the element; the result of a binary operator 
operating on two operands will be the data type of the operand having the 
highest type precedence of the two.  If the two operands are of type double 
precision and type complex, respectively, the data type of the result will be 
double precision complex. 
The data type of an expression enclosed in parentheses (a subexpression) is 
not affected by the context in which the subexpression appears.  The data type 
of the value of a subexpression is the data type of the last operation 
performed.  It is the data type of the highest type precedence in the 
subexpression.  The data type of the result of each operation in a 
subexpression is dependent only on the data types of the two operands.  When 
the two operands for an operation are of differing data types, the operand 
whose data type is of the lower type precedence is converted to the higher 
type precedence. 
In the case of a complex operand and a double precision operand, the data 
types differ and the type precedences are equal.  This causes both operands to 
be converted to double precision complex type. 
Examples: 
Some examples of expressions and their resulting values follow. 
    Expression              Value           Type of Result 
    ----------              -----           -------------- 
    3/4                     0               Integer 
    (3/4)                   0               Integer 
    1.0*3/4                 0.75            Real 
    3/4*1.0                 0.0             Real 
    1.0*(3/4)               0.0             Real 
    (3/4)*1.0               0.0             Real 
    1.0D0+1.0+285212417     285212419.0D0   Double precision 
    (1.0D0+1.0)+285212417   285212419.0D0   Double precision 
    (1.0D0+285212417)+1.0   285212419.0D0   Double precision 
    1.0D0+(1.0+285212417)   285212417.0D0   Double precision 
NOTE:  This last result is due to the loss of significance when converting the 
integer value to a real value.  Conversion of an integer value to a double 
precision value does not lose significant digits. 
    0.0+285212417           285212416.0    Real 
The following example demonstrates the possible loss of significance in a 
mixed-mode expression. 
    1.5D0+1.5+2             5.0D0          Double precision 
With no loss of significant digits, the value of the result is the value 
expected. 
Rules: 
The following rules also apply to mixed expressions: 
1.  Expressions appearing as subscripts of function arguments are independent 
    of the expression in which the array or function appears.  The subscript 
    or argument expressions are evaluated in their own mode and do not affect, 
    nor are they affected by, the mode of the outer expression. 
2.  Integer expressions that appear as exponents (i.e., to the right of an ** 
    operator) are evaluated in their own mode; that is, integer. 
3.  Integer, real, and double precision values that appear in complex or 
    double precision complex expressions are assumed to have imaginary parts 
    of zero. 
4.  Values of expressions, subexpressions, and elements may not exceed the 
    limits associated with the mode of the expression. 
00001
00002
The normal entry into a subprogram is at the SUBROUTINE or FUNCTION statement 
that defines it, with execution beginning at the first executable statement 
following the SUBROUTINE or FUNCTION statement.  It is also possible to enter 
a subprogram at some other point, by using the ENTRY statement. 
NAMELIST_STATEMENT
    Alternate NAMELIST Statement: 
Syntax: 
NAMELIST/x1/list1/x2/list2/,...,/xn/listn 
Parameters: 
each xi    is a NAMELIST name 
list    is a list of the form 
    v1,v2,...,vn 
    where 
    each vi     is a variable or array identifier. 
Description: 
This statement specifies the list of variables and arrays that are available 
to the corresponding namelist I/O statement. 
The NAMELIST names defined in a program unit are independent of those defined 
in any other program units.  The following rules apply to using a NAMELIST 
name: 
1.  A NAMELIST name is a symbolic name that must not be the same as any 
    variable name, array name, or subprogram name. 
2.  A NAMELIST name is enclosed in slashes.  The list of variable or array 
    names belonging to a NAMELIST name is completed when a new NAMELIST name 
    enclosed in slashes or the end of the NAMELIST statement is encountered. 
3.  A variable or array name may belong to one or more NAMELIST lists. 
4.  A NAMELIST name may be declared in a NAMELIST statement and may be 
    declared only once per program unit.  After its declaration, the NAMELIST 
    name may appear only in input/output statements. 
5.  The NAMELIST statement must appear in the FORTRAN program prior to its use 
    in a READ/WRITE statement.  It should appear prior to the first executable 
    statement. 
6.  The rules for input/output conversion of NAMELIST data are the same as the 
    rules for data conversion. 
00003
00002
00001
Syntax: 
READ (u,x[,ERR=k1][,END=k2])) READ x 
Parameters: 
u    is the logical unit number which may be any integer, real, or double 
precision expression, truncated to integer if necessary. 
x    is a NAMELIST name. 
k1,k2    are statement labels 
Description: 
The NAMELIST READ and NAMELIST WRITE statements differ from the two forms of 
the INPUT statement and the OUTPUT statement in that each NAMELIST has a 
unique name by which it can be referenced. 
Note that I/O lists are not used with these types of I/O statements.  Neither 
should the BACKSPACE command be used with units being used in this manner. 
After the NAMELIST statement declares a name "x" to refer to a particular list 
of variables or array names, the READ (u,x), or WRITE (u,x) statement can be 
used to transmit data between the unit associated with "u" and the variables 
or array specified by the NAMELIST name "x".  See NAMELIST_STATEMENT.  Data is 
input via the READ (u,x) statement.  The first character of the first record 
of a group to be read must be an ampersand (&) followed immediately by the 
NAMELIST name.  The NAMELIST name must be followed by a blank and must not 
contain any embedded blanks.  This name is followed by data items separated by 
commas.  (A comma after the last item is optional if the &END is on the 
following record).  The end of an input group is signaled by &END.  The two 
forms of the data items in an input record are 
o   symbolic name=constant 
    The symbolic name may be an array element name or a variable name. 
    Subscripts must be integer constants.  The constant may be integer, real, 
    double precision, complex, double complex, logical, character, or literal. 
    (Logical constants may have the form .TRUE. or T and .FALSE. or F.) 
o   array name=set of constants (separated by commas) 
    The set of constants may contain any of the types stated above.  The 
    number of constants must be less than or equal to the number of elements 
    in the array.  Successive occurrences of the same constant can be 
    represented in the form k* constant, where k is a nonzero integer constant 
    specifying the number of times the constant is to occur. 
The variable names and array names specified in the input data must appear in 
the NAMELIST list, but the order is not significant.  A name that has been 
equivalenced to a name in the input data cannot be substituted for that name 
in the NAMELIST list.  The NAMELIST list may contain COMMON variable names but 
cannot contain dummy argument names. 
Each data record must begin with a blank character followed by a complete 
variable name, or array name, or constant. 
Embedded blanks are not permitted in names or constants.  Trailing blanks 
after integers and exponents are treated as null. 
Usage Notes: 
When a NAMELIST WRITE statement is used, data is output in a form that can be 
read by a NAMELIST READ statement.  All identifiers specified in the NAMELIST 
list are output, each according to its type.  The form of the data is as 
discussed above under input, e.g., variable=constant value, or array=set of 
constant values.  The data fields are large enough to contain all the 
significant digits.  The values of a complete array are written out in columns 
(not rows as with the OUTPUT statement). 
Examples: 
    DIMENSION B(3),I(3,3),K(3,3) 
    DATA I/9*0/,C/543E3/ 
    NAMELIST/NAM1/A,B,I,J,K/NAM2/C,J,I,K 
    READ (105,NAM1) 
    WRITE (108,NAM2) 
The NAMELIST statement defines two NAMELIST lists, NAM1 and NAM2.  The READ 
statement causes input to be read from unit 105 into the variables and arrays 
associated with NAM1.  The data records to be read are in the following form: 
    First record     &NAM1 I(2,3)=5,J=4,A=3.2 
    Second record    B(3)=4.0,K=2,3,7*4,&END 
The NAMELIST name is read off the first record and is checked so that it 
agrees with the name in the READ statement.  If the names do not agree or if 
the name is missing, the next record is read looking for &name or &END.  In 
the example, a match is found and the input data is processed.  The integer 
constants 5 and 4 are put into I(2,3) and J respectively.  The real values 3.2 
and 4.0 are put into A and B(3) respectively.  Since K is an array name not 
followed by a subscript, the entire array K is filled with the succeeding 
constants.  That is, K(1,1)=2, K(2,1)=3, and 4 is placed in elements K(3,1) 
through K(3,3). 
The WRITE statement causes data to be output from the variables associated 
with NAMELIST NAM2 to unit 108.  In our example, we assume that the variables 
input via the READ statement are not altered and the variable C has a value 
543E3 stored in it.  Therefore, the WRITE statement would output the following 
records: 
    First record    &NAME2 
    2nd      "      C=543000.0 
    3rd      "      J=4 
    4th      "      I=0 
    5th      "        0 
    6th      "        0 
    7th      "        0 
    8th      "        0 
    9th      "        0 
    10th     "        0 
    11th     "        5 
    12th     "        0 
    13th     "      K=2 
    14th     "        3 
    15th     "        4 
    16th     "        4 
    17th     "        4 
    18th     "        4 
    19th     "        4 
    20th     "        4 
    21st     "        4 
    22nd  record    &END 
00002
00004
00001
00003
Syntax: 
NAMELIST v1,v2,v3,...,vn 
Parameters: 
n>0 
vi    are variable or array identifiers.  Dummy identifiers and VIRTUAL 
entities may not appear. 
When an array name appears in a NAMELIST declaration, all elements of the 
array may be processed by an INPUT statement. 
Description: 
The NAMELIST statement is used to define the variables that may be processed 
by INPUT statements in a program unit.  A NAMELIST statement with no 
identifiers following it causes all appropriate variables that appear in the 
program to be placed in the name list; i.e., all nondummy variables and 
arrays.  This can be helpful during program checkout, since it enables the 
user to input any variable without knowing, at compile time, which variable it 
will be desirable to input. 
The NAMELIST variables defined in a program unit are independent of those 
defined in any other program units.  Each program has its own NAMELIST.  This 
means that 
1.  A variable that appears in one program may not be processed by an INPUT 
    statement in another program unless, for example, the variable is in 
    COMMON and also appears in the other program. 
2.  If two or more programs have separate variables with the same name, it is 
    possible to input into either of them.  It simply depends on which program 
    is performing the input.  There is no conflict between the separate 
    NAMELISTs. 
Examples: 
    NAMELIST T,G,I,F,RATE,COUNT,ITEM,QUANTITY 
    NAMELIST JOHN,VECTOR,XRAY 
    NAMELIST 
00002
00003
00001
Two types of NAMELIST statements are available: 
1.  The NAMELIST statements used to define the variables that may be processed 
    by INPUT statements in a program unit.  These are the NAMELIST statement 
    and the Alternate NAMELIST statement.  (See NAMELIST_STATEMENT and 
    ALTERNATE_NAMELIST_STATEMENT.) 
2.  Another form used to declare the namelist names that appear in namelist 
    READ/WRITE statements.  (See NAMELIST_READ and NAMELIST_WRITE.) 
Syntax: 
WRITE(u,x[,ERR=k1]) 
Parameters: 
u    is the logical unit number which may be any integer, real, or double 
precision expression, truncated to integer if necessary. 
x    is a NAMELIST name. 
k1,k2    are statement labels 
Description: 
The NAMELIST READ and NAMELIST WRITE statements differ from the two forms of 
the INPUT statement and the OUTPUT statement in that each NAMELIST has a 
unique name by which it can be referenced. 
Note that I/O lists are not used with these types of I/O statements.  Neither 
should the BACKSPACE command be used with units being used in this manner. 
After the NAMELIST statement declares a name "x" to refer to a particular list 
of variables or array names, the READ (u,x), or WRITE (u,x) statement can be 
used to transmit data between the unit associated with "u" and the variables 
or array specified by the NAMELIST name "x".  (See NAMELIST_STATEMENT).  Data 
is input via the READ (u,x) statement.  The first character of the first 
record of a group to be read must be an ampersand (&) followed immediately by 
the NAMELIST name.  The NAMELIST name must be followed by a blank and must not 
contain any embedded blanks.  This name is followed by data items separated by 
commas.  (A comma after the last item is optional if the &END is on the 
following record).  The end of an input group is signaled by &END.  The two 
forms of the data items in an input record are 
o   symbolic name=constant 
    The symbolic name may be an array element name or a variable name. 
    Subscripts must be integer constants.  The constant may be integer, real, 
    double precision, complex, double complex, logical, character, or literal. 
    (Logical constants may have the form .TRUE. or T and .FALSE. or F.) 
o   array name=set of constants (separated by commas) 
    The set of constants may contain any of the types stated above.  The 
    number of constants must be less than or equal to the number of elements 
    in the array.  Successive occurrences of the same constant can be 
    represented in the form k* constant, where k is a nonzero integer constant 
    specifying the number of times the constant is to occur. 
The variable names and array names specified in the input data must appear in 
the NAMELIST list, but the order is not significant.  A name that has been 
equivalenced to a name in the input data cannot be substituted for that name 
in the NAMELIST list.  The NAMELIST list may contain COMMON variable names but 
cannot contain dummy argument names. 
Each data record must begin with a blank character followed by a complete 
variable name, or array name, or constant. 
Embedded blanks are not permitted in names or constants.  Trailing blanks 
after integers and exponents are treated as null. 
Usage Notes: 
When a NAMELIST WRITE statement is used, data is output in a form that can be 
read by a NAMELIST READ statement.  All identifiers specified in the NAMELIST 
list are output, each according to its type.  The form of the data is as 
discussed above under input, e.g., variable=constant value, or array=set of 
constant values.  The data fields are large enough to contain all the 
significant digits.  The values of a complete array are written out in columns 
(not rows as with the OUTPUT statement). 
Examples: 
    DIMENSION B(3),I(3,3),K(3,3) 
    DATA I/9*0/,C/543E3/ 
    NAMELIST/NAM1/A,B,I,J,K/NAM2/C,J,I,K 
    READ (105,NAM1) 
    WRITE (108,NAM2) 
The NAMELIST statement defines two NAMELIST lists, NAM1 and NAM2.  The READ 
statement causes input to be read from unit 105 into the variables and arrays 
associated with NAM1.  The data records to be read are in the following form: 
    First record     &NAM1 I(2,3)=5,J=4,A=3.2 
    Second record    B(3)=4.0,K=2,3,7*4,&END 
The NAMELIST name is read off the first record and is checked so that it 
agrees with the name in the READ statement.  If the names do not agree or if 
the name is missing, the next record is read looking for &name or &END.  In 
the example, a match is found and the input data is processed.  The integer 
constants 5 and 4 are put into I(2,3) and J respectively.  The real values 3.2 
and 4.0 are put into A and B(3) respectively.  Since K is an array name not 
followed by a subscript, the entire array K is filled with the succeeding 
constants.  That is, K(1,1)=2, K(2,1)=3, and 4 is placed in elements K(3,1) 
through K(3,3). 
The WRITE statement causes data to be output from the variables associated 
with NAMELIST NAM2 to unit 108.  In our example, we assume that the variables 
input via the READ statement are not altered and the variable C has a value 
543E3 stored in it.  Therefore, the WRITE statement would output the following 
records: 
    First record    &NAME2 
    2nd      "      C=543000.0 
    3rd      "      J=4 
    4th      "      I=0 
    5th      "        0 
    6th      "        0 
    7th      "        0 
    8th      "        0 
    9th      "        0 
    10th     "        0 
    11th     "        5 
    12th     "        0 
    13th     "      K=2 
    14th     "        3 
    15th     "        4 
    16th     "        4 
    17th     "        4 
    18th     "        4 
    19th     "        4 
    20th     "        4 
    21st     "        4 
    22nd  record    &END 
00002
00004
00001
00003
When a NAMELIST WRITE statement is used, data is output in a form that can be 
read by a NAMELIST READ statement.  All identifiers specified in the NAMELIST 
list are output, each according to its type.  The form of the data is as 
discussed above under input, e.g., variable=constant value, or array=set of 
constant values.  The data fields are large enough to contain all the 
significant digits.  The values of a complete array are written out in columns 
(not rows as with the OUTPUT statement). 
Examples: 
    DIMENSION B(3),I(3,3),K(3,3) 
    DATA I/9*0/,C/543E3/ 
    NAMELIST/NAM1/A,B,I,J,K/NAM2/C,J,I,K 
    READ (105,NAM1) 
    WRITE (108,NAM2) 
The NAMELIST statement defines two NAMELIST lists, NAM1 and NAM2.  The READ 
statement causes input to be read from unit 105 into the variables and arrays 
associated with NAM1.  The data records to be read are in the following form: 
    First record     &NAM1 I(2,3)=5,J=4,A=3.2 
    Second record    B(3)=4.0,K=2,3,7*4,&END 
The NAMELIST name is read off the first record and is checked so that it 
agrees with the name in the READ statement.  If the names do not agree or if 
the name is missing, the next record is read looking for &name or &END.  In 
the example, a match is found and the input data is processed.  The integer 
constants 5 and 4 are put into I(2,3) and J respectively.  The real values 3.2 
and 4.0 are put into A and B(3) respectively.  Since K is an array name not 
followed by a subscript, the entire array K is filled with the succeeding 
constants.  That is, K(1,1)=2, K(2,1)=3, and 4 is placed in elements K(3,1) 
through K(3,3). 
The WRITE statement causes data to be output from the variables associated 
with NAMELIST NAM2 to unit 108.  In our example, we assume that the variables 
input via the READ statement are not altered and the variable C has a value 
543E3 stored in it.  Therefore, the WRITE statement would output the following 
records: 
    First record    &NAME2 
    2nd      "      C=543000.0 
    3rd      "      J=4 
    4th      "      I=0 
    5th      "        0 
    6th      "        0 
    7th      "        0 
    8th      "        0 
    9th      "        0 
    10th     "        0 
    11th     "        5 
    12th     "        0 
    13th     "      K=2 
    14th     "        3 
    15th     "        4 
    16th     "        4 
    17th     "        4 
    18th     "        4 
    19th     "        4 
    20th     "        4 
    21st     "        4 
    22nd  record    &END 
00001
SYMBOLIC_NAMES
A DO loop may include another DO loop.  That is, DO loops may be nested; 
however, they may not be overlapped.  In a nest of DO loops the same statement 
may be used as the terminal statement for any number of DO ranges; a transfer 
of control to this terminal statement may be made only from the innermost DO 
loop.  There is no limit to the number of DO ranges that may be nested. 
Subexpressions have been defined as expressions enclosed in parentheses.  It 
is also possible to have nested subexpressions as in 
    X*(Z+Y*(H-G/(Y+D)-W)+M(8)) 
where (Y+D) may be called the innermost subexpression, and (H-G/(Y+D)-W) is 
the next innermost subexpression.  Where all the elements in an expression are 
the same data type, the evaluation hierarchy is: 
1.  The innermost subexpression, followed by the next innermost subexpression, 
    until all subexpressions have been evaluated. 
2.  Within a subexpression, the arithmetic operations, in the following order 
    of precedence: 
   Operation            Operator    Order 
   ---------            --------    ----- 
   Exponentiation       **          1 (highest) 
   Multiplication       *           2 
      and Division      / 
   Addition and         +           3 
      Subtraction       - 
      (or Positive 
      and Negative) 
It is not permissible to nest relational expressions, as in the case 
    (L<(X.  GT.  0.2345E6)) 
where (X.  GT.  0.2345E6) is a relational subexpression, rather than an 
arithmetic subexpression, as the definition of relational expressions 
requires. 
When two character expressions are compared using a relational operator, the 
lengths of the two expressions may differ.  If so, the shorter of the two 
expressions is considered to have blanks concatenated on the right to the 
length of the longer expression.  The value of e1 is considered to be less 
than the value of e2 if the value of e1 precedes the value of e2 in the 
collating sequence.  Note that the collating sequence may be defined 
differently by each system, and that comparisons other than .EQ. and .NE.  may 
not yield the same result when run on other FORTRAN systems. 
NOTE:  The functions LGE, LGT, LLE, and LLT should be used to compare 
character expressions in a machine/character set independent fashion.  For 
details on these functions, enter HELP (FORTRAN) INTRINSICS_GENERICS . 
00001
CHANGES
Syntax: 
NORMAL I1,I2,I3,...In 
Parameters: 
Ii    is an external function name. 
n > = 0. 
Description: 
The NORMAL statement is provided to allow object code optimization by letting 
external functions with no side effects to be declared.  The compiler 
currently treats all external functions as abnormal.  A "normal" function does 
not perform any of the following actions: 
1.  Modify any of the calling arguments. 
2.  Modify a COMMON or GLOBAL variable. 
3.  Perform any sort of input/output. 
4.  Return different values when called with the same argument values. 
5.  Keep a chronological count or history. 
00002
00001
Notation Conventions used in command specifications and examples are listed 
below. 
  Notation             Description 
Lower-case letters   Lower-case letters identify an element that must 
                     be replaced with a user-selected value. 
                     ASSIGN s TO i    can be entered as 
                     ASSIGN 5 TO JUMP 
CAPITAL LETTERS      Capital letters must be entered as shown for 
                     input, and will be printed as shown in output. 
                     CALL p    means enter CALL followed by the value 
                     for p. 
[ ]                  An element inside brackets is optional. 
                     [,ERR=k]    means the ERR parameter may be 
                     included. 
{ }                  Elements placed inside a pair of braces identify 
                     a required choice. 
|                    The Or bar also separates elements in a required 
                     or optional choice. 
                     {A|id}    means that either the letter A or 
                     the value of id must be entered. 
Special Characters   Special characters appear as shown in output 
                    messages and must be entered as shown when input. 
apostrophe           STOP 's'    means that the value for s must be 
                     entered enclosed in apostrophes, e.g.: 
                     STOP 'JOBENDED' 
Subscript numbers    Parameter names are numbered to indicate a first, 
and horizontal       second, etc., representation of the same parameter. 
ellipsis (...) 
                     Ellipses are used to indicate that parameter values 
                     can be repeated.  The parameter may be assigned the 
                     same or a different value for each occurrence. 
                     v1,v2,...vn    means that multiple values for v 
                     can be entered, separated by commas. 
Vertical Ellipsis    Vertical ellipses are used in program and output 
         .           examples to indicate portions have been omitted. 
         . 
         . 
b                    The letter b is used in input and output examples 
                     to indicate the blank character. 
Syntax: 
OPEN ([UNIT=]u[,{NAME|FILE}=fin][,ACCESS=ax][,ACCOUNT=an][,BLANK=blnk] 
  [,ERR=s][,EXTEND=xte][,FORM=fm][,IOSTAT=ios][,KEYM=kl][,NRECS=nr] 
  [,RECL=rl][,STATUS=typ][,USAGE=use]) 
Parameters: 
[UNIT]=u[,{NAME|FILE}=fin]...    are parameters described in the following 
table.  Enter ? or ?? to view the options, or enter:  HELP (FORTRAN) OPEN 
option_name for example, HELP (FORTRAN) OPEN USAGE 
  Option                    Description 
  ACCESS=ax 
                            ax    is a character expression that must have a 
                            value of 'TERMINAL', 'DIRECT', 'SEQUENTIAL', 
                            'KEYED', 'RANDOM', or 'RELATIVE'.  The TERMINAL 
                            access is only applicable to devices.  DIRECT 
                            specifies the file as being a direct-access file. 
                            SEQUENTIAL specifies a sequential file, KEYED a 
                            keyed file, RANDOM a random file, and RELATIVE, a 
                            relative file.  For an existing file, this 
                            specification must agree with the actual file.  If 
                            this specifier is omitted, the assumed value is 
                            'SEQUENTIAL'. 
                            Note:  Access for the OPEN statement is not 
                            necessarily the same as for the CP-6 SET command. 
  ACCOUNT=an 
                            an    is a character expression that is the name 
                            of the account to be associated with the file 
                            name.  This parameter should only be used when 
                            'INPUT' is used with the 'USAGE' option and/or 
                            'OLD' is used with the 'STATUS' option.  Account 
                            can also be specified as part of the NAME= 
                            specifier, in which case ACCOUNT=an is omitted. 
  BLANK=blnk 
                            blnk    is a character expression that must have a 
                            value of 'NULL' or 'ZERO'.  If 'NULL' is 
                            specified, all blank characters in 
                            numeric-formatted input fields on the specified 
                            unit are ignored, except that a field of all 
                            blanks has a value of zero.  If 'ZERO' is 
                            specified, all blanks other than leading blanks 
                            are treated as zeros.  If this specifier is 
                            omitted, then the effect is as if BLANK='NULL' was 
                            specified. 
  ERR=s 
                            s    is a statement label for the error specifier. 
  EXTEND=xte 
                            xte    is a character expression that must have 
                            the value of 'YES' or 'NO'.  It is used to create 
                            OPENs of labeled tape files.  If 'YES' is used, 
                            the selected volume is positioned after the last 
                            file.  If 'NO' is used, the next file will be 
                            created starting at the current volume position. 
                            This parameter is intended for use only with 
                            labeled tape files.  If this parameter is omitted 
                            for a labeled tape file that is being created, the 
                            default value is 'NO'. 
  FORM=fm 
                            fm    is a character expression that must have a 
                            value of 'FORMATTED' or 'UNFORMATTED'.  For a 
                            sequential or direct access file that is being 
                            created, this specifier determines whether the 
                            records of the file are all formatted or all 
                            unformatted.  If the file exists, the specifier 
                            must agree with the actual attribute of the file. 
                            If this specifier is omitted for an existing file, 
                            the actual attribute of the file will be assumed. 
                            If this specifier is omitted for a file being 
                            created, the value 'UNFORMATTED' is assumed. 
  IOSTAT=ios 
                            ios    is an input/output status specifier.  It is 
                            an integer variable or an array ID.  Execution of 
                            an OPEN statement causes ios to be set to a zero 
                            value if no error occurred.  If an error occurred 
                            during the opening of the unit, the location ios 
                            will be set to the appropriate error code.  (See 
                            VALUES_OF_IOS) 
  KEYM=kl 
                            kl    is an integer, real, or double precision 
                            expression whose value is converted to an integer 
                            according to the rules for arithmetic assignment 
                            statements.  It specifies the maximum length of 
                            the key in a keyed access file.  The length is the 
                            number of characters in the key.  If the file 
                            exists and kl does not agree with the actual key 
                            length of the records, an error condition results. 
                            If the file is being created, kl specifies the 
                            maximum key length attribute.  The value kl must 
                            be in the range from 3 through 31.  The KEYM= 
                            option is permitted only when the ACCESS='KEYED' 
                            option is present.  If this specifier is omitted 
                            for a keyed access file, the default value of 3 is 
                            assumed.  For direct-access files, the value 3 is 
                            always assumed.  If the file is a RELATIVE file, 
                            the KEYM field specifies the maximum number of 
                            records that can be contained in the file. 
  {NAME|FILE}=fin 
                            fin    is a character expression that names the 
                            file to be connected to the specified unit.  An 
                            error condition exists if the filename is not 
                            acceptable.  If this specifier is omitted, the 
                            specified unit is connected to a temporary file 
                            which does not yet exist if the unit is not 
                            currently open.  Note that either NAME=fin or 
                            FILE=fin is permitted.  The account may be 
                            specified as part of the NAME= specifier.  This 
                            form is commonly called a fid and appears as: 
                                name.account.password 
  NRECS=nr 
                            nr    is the number of records that are to be 
                            allowed in a RELATIVE file.  This parameter is 
                            only meaningful if the specified file access is 
                            RELATIVE. 
  RECL=rl 
                            rl    is an integer, real, or double precision 
                            expression whose value is converted to an integer 
                            according to the rules for arithmetic assignment 
                            statements.  It specifies the length of each 
                            record in a file.  If the records are formatted, 
                            the length is the number of characters.  If the 
                            records are unformatted, the length is the number 
                            of words.  If the file is being created, rl 
                            specifies the record length attribute.  For 
                            formatted records, if rl is left unspecified, the 
                            default maximum record length is 133 characters. 
  STATUS=typ 
                            typ    is a character expression that must have a 
                            value of 'OLD', 'NEW', 'SCRATCH', or 'UNKNOWN'. 
                            If 'OLD' or 'NEW' is specified, a name specifier 
                            must be given.  If 'OLD' is specified, the file 
                            must be an existing file; otherwise, an error 
                            condition exists.  If 'NEW' is specified, the file 
                            must be nonexisting; otherwise, an error condition 
                            exists.  If 'SCRATCH' is specified, the name is 
                            disregarded and the file is connected to the 
                            specified unit for use by the executable program, 
                            but is deleted either at the execution of a CLOSE 
                            statement referring to the same unit or at the 
                            termination of the executable program.  A value of 
                            'UNKNOWN' is assumed if 'STATUS=typ' is not 
                            specified. 
  [UNIT=]u 
                            u    is an external unit specifier.  It may be a 
                            keyword form or the positional form. 
  USAGE=use 
                            use    is a character expression that must have 
                            one of the following values:  'INPUT', 
                            'INPUT,SHARED', 'OUTPUT', 'UPDATE', 
                            'UPDATE,SHARED', or 'CREATE'.  If 'INPUT' or 
                            'INPUT,SHARED' is used, the ACCOUNT option also 
                            may be used to specify an alternate account for 
                            input; only the STATUS options 'OLD' and 'UNKNOWN' 
                            may be used.  If 'OUTPUT' is used, the STATUS 
                            option 'SCRATCH' must not be used.  If 'CREATE' is 
                            used, any STATUS option may be used.  If 'UPDATE' 
                            or 'UPDATE,SHARED' is used, only the STATUS 
                            options 'OLD' or 'UNKNOWN' may be used. 
                            USAGE=use can be used to denote CP-6 shared file 
                            usage.  The equivalent USAGE options and SET 
                            options of the IBEX processor are as follows: 
                            OPEN Statement          IBEX SET Options 
                            --------------          ---------------- 
                            USAGE='INPUT'           FUN=IN, SHARE=IN 
                            USAGE='INPUT,SHARED'    FUN=IN, SHARE=ALL 
                            USAGE='UPDATE,SHARED'   FUN=UPDATE, SHARE=ALL 
                            For additional information on file sharing, enter 
                            HELP (IBEX) SET option . 
Description: 
The following rules apply to use of the OPEN statement: 
1.  The unit specifier is required; all other specifiers are optional, except 
    that the record length rl must be specified if the file is being created 
    and the access is 'DIRECT'.  As noted above, however, some of the 
    specifications have an assumed value if they are omitted. 
2.  If a unit is connected to a file that exists, execution of an OPEN 
    statement for that unit is permitted.  If the FILE= (or NAME=) specifier 
    is not included in the OPEN statement, the file to be connected to the 
    unit is the same as the file to which the unit is connected. 
3.  If the file to be connected to the unit does not exist, but is the same as 
    the file to which the unit is preconnected, the properties specified by 
    the OPEN statement become a part of the connection. 
4.  If the file to be connected to the unit is not the same as the file to 
    which the unit is connected, the effect is as if a CLOSE statement without 
    a status specifier had been executed for the unit immediately prior to the 
    execution of the OPEN statement. 
5.  If the file to be connected to the unit is the same as the file to which 
    the unit is connected, only the BLANK= specifier may have a value 
    different from the one currently in effect.  The position of the file is 
    unaffected. 
6.  If a file is connected to a unit, execution of an OPEN statement on that 
    file and a different unit is not permitted. 
7.  A unit may be connected by execution of an OPEN statement in any program 
    unit of an executable program, and once connected, may be referenced in 
    any program unit of that executable program. 
The following table lists functions of the STATUS= and USAGE= specifiers under 
various conditions for disk and tape files.  STATUS USAGE FUNCTION ------ 
----- -------- 
  STATUS=   USAGE=    Function 
UNKNOWN   OUTPUT    If file exists, 
                    Disk File - deletes old file, replaces old file 
                    at completion of the job, sets EXIST option 
                    to NEWFILE and changes the function to CREATE, and 
                    opens the old file (actually newly-created file). 
                    Tape File - opens the file and changes the 
                    function to UPDATE, and remains at current 
                    position in the file. 
                    If file does not exist, 
                    Disk file - creates a file with function CREATE. 
                    Tape file - creates a file with function CREATE at 
                    end of labeled tape. 
---------------------------------------------------------------------- 
UNKNOWN   CREATE    Same as STATUS=UNKNOWN, USAGE=OUTPUT 
                    CREATE and OUTPUT are considered to have same 
                    function. 
                    When STATUS= UNKNOWN and we cannot open as old or 
                    new file, will open as a scratch file with 
                    function CREATE. 
---------------------------------------------------------------------- 
UNKNOWN   UPDATE    If file exists, 
                    Disk file - opens file with function of UPDATE and 
                    positions to the end of the file. 
                    Tape file - opens with function of UPDATE and 
                    positions to the end of the file. 
                    If file does not exist, 
                    Disk file - creates a new file. 
                    Tape file - creates a new file at end of labeled 
                    tape. 
---------------------------------------------------------------------- 
UNKNOWN   INPUT     If file exists, 
                    Disk file - opens file with function of INPUT. 
                    Tape file - opens file with function of INPUT. 
                    If file doesn't exist, 
                    Disk file - creates a new file; function is changed 
                    from INPUT to CREATE. 
                    Tape file - creates new file with function CREATE 
                    at the end of the labeled tape. 
                    If USAGE= INPUT and a new file cannot be opened, an 
                    error message will be generated. 
00002
00003
00004
00016
00005
00006
00010
00007
00008
00009
00010
00011
00001
00012
00016
00013
00014
00015
OPEN
LOGICAL_OPERATORS
RELATIONAL_OPERATORS
COMPILER_OPTIONS
OPTIONAL_SIZE_SPECS
Optional Size Specifications 
In addition to the standard type declarations (INTEGER, REAL, etc.), Bull 
FORTRAN-77 also permits an optional form that specifically defines the memory 
size of the data.  This option is written as 
*n 
where 
n    is the number of bytes occupied by the data.  (There are four bytes in a 
word.)  There is an optional form for each of the type declarations, as shown 
below: 
    Standard Declaration       Optional Form 
    --------------------       ------------- 
    INTEGER                    INTEGER*4 
    REAL                       REAL*4 
    DOUBLE PRECISION           REAL*8 
    COMPLEX                    COMPLEX*8 
    DOUBLE COMPLEX             COMPLEX*16 
    LOGICAL                    LOGICAL*4 
    INTEGER*2 
    LOGICAL*1 
Note that this form of specification is extremely machine dependent.  On 
machines with different word or byte sizes, the numbers used to specify size 
would have a significantly different meaning.  Therefore, to retain 
compatibility with future systems (and others currently existing), it is 
recommended that these forms be avoided, especially since no additional 
capability is provided by them. 
The *n modifier may appear in three kinds of statements:  IMPLICIT statements, 
FUNCTION statements, and explicit type statements.  The position of the *n, 
relative to the type declaration that it modifies, depends on the statement, 
as follows: 
1.  In the IMPLICIT statement, the *n is appended to the type declaration 
    word, as in 
      IMPLICIT REAL*8(I-K),INTEGER*4(A-H),LOGICAL(L,N) 
2.  In the FUNCTION statement, the *n is appended to the name of the function, 
    rather than to the type word, as in 
      REAL FUNCTION MULT*8(X,Y,Z) 
      COMPLEX FUNCTION CINVERT*16(C) 
3.  In explicit type statements, the *n can be appended to the type word, or 
    to the identifiers being declared, or to both.  When appended to the type 
    word, the *n holds for all identifiers listed, except for those with an 
    individual size specification of their own.  In other words, the *n 
    appended to an identifier takes precedence over the *n applying to the 
    whole statement.  For example, 
      COMPLEX*8 CUM,LAUDE*16 
      LOGICAL FLAG(10),TRUTH*4(10) 
      REAL*8$8*4(8)/8*4/ 
OPTIONAL_SIZE_SPECS
COMPILER_OPTIONS
COMPILER_OPTION_GROUPS
Syntax: 
OUTPUT [,]k  or  OUTPUT (u)[,]k 
Parameters: 
k    is an output list, consisting of variables, arrays, expressions, and/or 
character constants. 
u    is the unit number on which to perform the output.  This is normally an 
integer constant or variable.  It also may be any integer, real, or double 
precision expression, truncated to integer if necessary.  If no unit number is 
specified, the output will automatically be on unit 108 (the standard print 
unit), unless the US compiler option is specified, in which case 6 is the 
default. 
Description: 
The name of each list item will be output, followed by an equal sign and the 
value of the item, in an appropriate format.  For example: 
    OUTPUT X,Y,X+Y,SQRT(X**2+Y**2) 
might produce the following lines of output: 
    X = .500000 
    Y = 1.20000 
    X+Y = 1.70000 
    SQRT(X**2+Y**2) = 1.30000 
Complex values are output as complex constants; the other data types are also 
output in natural forms, as shown in the following example: 
    DOUBLE PRECISION D 
    COMPLEX C 
    LOGICAL L 
    OUTPUT(6),I,R,D,C,L 
which might result in these lines: 
    I = 79 
    R = 4370.72 
    D = 99301.3922310385 
    C = (-56.2234,334.882) 
    L = T 
Each value is written on a separate line, beginning in column 2 (so that no 
carriage control will take place).  The maximum width of any line is dependent 
on the logical unit.  If the unit is connected to a line printer, the maximum 
number of characters is 132. 
Usage Notes: 
If the first item in the output list begins with a left parenthesis, and no 
unit number is specified, there must be a comma after the word OUTPUT. 
Otherwise, the list item will appear to be a unit number.  For example, 
    OUTPUT,(A+B),X(I) 
When an unsubscripted array name appears in the output list, the entire array 
is output in storage order (see ARRAY_STORAGE).  For example, 
    DIMENSION M(2,2),A(2) 
    OUTPUT M,A 
could print 
    M = 553 
        -4 
        0 
        11245 
    A = .472962 
        -33.0000 
To output headings or other alphanumeric information, a list item may be a 
character constant.  In this case, no equal sign or value will be generated; 
only the character string itself is output, as in 
    OUTPUT 'FINAL COORDINATES' 
which generates the line 
    FINAL COORDINATES 
Implied DO loops may be used, as in the following example.  Note that the list 
begins with a parenthesis and is therefore preceded by a comma. 
    OUTPUT,(K,A(K)/B(K),K=N1,N2) 
which could produce this output: 
    K = 3 
    A(K)/B(K) = 14.6135 
    K = 4 
    A(K)/B(K) = 15.0873 
    etc. 
Another feature is provided which enables OUTPUT records to be read 
subsequently by a NAMELIST INPUT statement.  (See INPUT_NAMELIST.)  An OUTPUT 
list item may consist of a single asterisk(*), which will cause the characters 
*END* to be output on a record.  This will cause an INPUT statement to 
terminate reading.  For example, 
    OUTPUT(4)X,I,J,AA,* 
The actual format specifications used to output the various types of data are 
shown below, although the programmer need not concern himself about them, 
since they are provided automatically.  Note that all of the formats are 
widthless. 
    Data Type                  Format Specifications 
    ---------                  --------------------- 
    INTEGER                    I 
    REAL                       1PG.6 
    DOUBLE PRECISION           1PG.15 
    COMPLEX                    1P,1H(G.6,-X,1H,G.6,-X,1H) 
    DOUBLE COMPLEX             1P,1H(G.15,-X,1H,G.15,-X,1H) 
    LOGICAL                    L 
    CHARACTER                  A,1X 
00002
00001
00003
LIST_DIRECTED_OUTPUT_RECORD
OUTPUT
NAMELIST_WRITE_OUTPUT_DATA
OVERFL_DVCHK
ARITHMETIC_OVERFLOW
Syntax: 
CALL OVERFL(status) 
CALL DVCHK(status) 
Parameters: 
status    is an integer variable into which a value is stored as specified 
under Description. 
Description: 
If a floating overflow has occurred, the status variable is set to 1, 
otherwise, it is set to 2.  If OVERFL or DVCHK is declared LOGICAL, it may 
also be referenced as a function, returning the value. TRUE. if overflow has 
occurred and .FALSE. if it has not.  The status variable should always be 
supplied.  The floating overflow trigger is always turned off after the test. 
00002
00001
                        The following list gives an overview of the major 
                        groups of topics provided by the FORTRAN HELP 
                        facility. 
                        ASSIGNMENT_STATEMENTS 
                        AUXILIARY_IO_STATEMENTS 
                        COMMON_LIBRARY 
                        COMPILER_OPTIONS 
                        CONSTANTS 
                        CONTROL_STATEMENTS 
                        DATA_TYPES 
                        DECLARATION_STATEMENTS 
                        EXPRESSIONS 
                        FEATURES 
                        FILES 
                        FORMATS 
                        FORTRAN_SOURCE_INFO 
                        FUNCTION_TYPES 
                        READ 
                        RECORDS 
                        SPECIFIERS 
                        SUBPROGRAM 
                        SYMBOLIC_NAMES 
                        WRITE 
                        Each of the above topics, when requested, will provide 
                        a further list of HELP topics available in that 
                        grouping.  For example: 
                            HELP (FORTRAN) ASSIGNMENT_STATEMENTS 
                        provides a list of the CP-6 FORTRAN-77 assignment 
                        statements: 
                            SIMPLE_ASSIGNMENT 
                            EXTENDED_ASSIGNMENT 
                        and 
                            HELP (FORTRAN) EXTENDED_ASSIGNMENT 
                        provides the syntax, parameters, description, etc., 
                        for the extended assignment statement. 
                        For a complete alphabetical list of FORTRAN HELP 
                        topics, type: 
                            HELP (FORTRAN) TOPICS 
                        For a summary of changes to FORTRAN included in this 
                        release, type: 
                            HELP (FORTRAN) CHANGES 
PARAMETER_STATEMENT
CPV_PARAMETER_STATEMENT
Syntax: 
PARAMETER (p1=e1,p2=e2,....pn=en) 
Parameters: 
pi    is a symbolic name. 
ei    is a constant expression or a constant (excepting literal, Hollerith, 
octal, or hexadecimal constants). 
Description: 
A PARAMETER statement is used to define the symbolic name of a constant.  If 
the symbolic name p is of type integer, real, double precision, or complex, 
the corresponding expression e must be an arithmetic constant expression.  If 
the symbolic name p is of type character or logical, the corresponding 
expression e must be a character constant expression or a logical constant 
expression, respectively. 
Each p is the symbolic name of a constant that becomes defined with the value 
determined from the expression e that appears on the right of the equal sign, 
in accordance with the rules for assignment statements; that is, the value 
from the expression is converted (if necessary) to the type of the symbolic 
name. 
Any symbolic name of a constant that appears in an expression e must have been 
defined previously in the same or a different PARAMETER statement in the same 
program unit. 
A symbolic name of a constant must not become defined more than once in a 
program unit. 
If a symbolic name of a constant is not of default implied type, its type must 
be specified by a type statement or IMPLICIT statement prior to its first 
appearance in a PARAMETER statement.  If the length specified for the symbolic 
name of a constant of type character is not the default length of one, its 
length must be specified in a type statement or IMPLICIT statement prior to 
the first appearance of the symbolic name of the constant.  Its length must 
not be changed by subsequent statements including IMPLICIT statements. 
Once such a symbolic name is defined, that name may appear in that program 
unit in any subsequent statement as a primary in an expression or in a DATA 
statement.  A symbolic name of a constant must not be part of a format 
specification.  A symbolic name of a constant must not be used to form part of 
another constant, for example, any part of a complex constant. 
A symbolic name in a PARAMETER statement may identify only the corresponding 
constant in that program unit. 
Examples: 
The following are examples of PARAMETER definitions: 
   CHARACTER C*4,C1*8 
   LOGICAL L1,L2,L3 
   PARAMETER (M=3,A=4.3) 
   PARAMETER (C='ABCD',C1='ABCDXYZ') 
   PARAMETER (L1= .TRUE. ,L2= .FALSE. ) 
   PARAMETER (N=5,J=3,ZERO=0.0) 
The following are examples of usage of the above definitions: 
   REAL A1(M,M) 
   CHARACTER*(N)C2*J 
   C2=C 
   DO 10 I=1,M 
   Z1(I,J)=ZERO 
10 CONTINUE 
(See PARAMETER_CPV.) 
00002
00003
00001
Within a FORMAT statement it is possible to repeat any number of 
specifications by enclosing them in parentheses, preceded by an optional 
repeat count, in the following form: 
r(S1,S2,S3,...,Sm) 
where r and the Si are defined previously, and m>1.  For example, the 
statement 
    3 FORMAT(3(A4,F.2,3X),3I) 
is equivalent to 
    3 FORMAT(A4,F.2,3X,A4,F.2,3X,A4,F.2,3X,3I) 
There is no limit to the number of repetitions of this form that can be 
present in a FORMAT statement. 
During input/output processing each repetitive specification is exhausted in 
turn, as is each singular specification. 
Examples: 
The following are additional examples of repetitive specifications: 
    34    FORMAT(4X,2(A8,X,7G.3),I4,3(D,L5)) 
    1125  FORMAT(/,R4,F.7,5(E14.8,2/),E14.8) 
    8     FORMAT(2(I8,2(3X,F12.9),F12.9),A16) 
In the last example above, repetitions have been nested.  Nesting of this type 
is permissible to a depth of ten levels. 
The presence of parenthesized groups within a FORMAT statement affects the 
manner in which the FORMAT is rescanned if more list items are specified than 
are processed the first time through the FORMAT statement.  In particular, 
when one or more such groups have appeared, the rescan begins with the group 
whose right parenthesis was the last one encountered prior to the final right 
parenthesis of the FORMAT statement.  A more complete discussion of this 
process is contained in LIST_INTERFACE under PRINT_POSITIONING, i.e., 
PRINT_POSITIONING LIST_INTERFACE. 
00001
Syntax: 
PAUSE [n|'s'] 
Parameters: 
n    is an unsigned decimal integer. 
's'    is a character constant of up to 72 characters. 
Description: 
This statement causes the program to cease execution temporarily, if online. 
Execution resumes after one character is input from the terminal.  Execution 
continues with the statement following the PAUSE. 
If an integer or a character constant is appended to the PAUSE statement, this 
value will be displayed at user terminal when the program pauses (online 
program only).  If in batch mode, a message prints, and execution continues. 
00002
00001
PAUSE
DUMP_PDUMP
PRECISION_OPTION
Automatic Double Precision can be specified in order to obtain more accuracy 
than is normally associated with real and complex operations.  This feature 
causes all data that would normally be real or complex to become double 
precision and double precision complex, respectively. 
Syntax: 
PRINT f [,k] 
    or 
PRINT *,k 
    or 
PRINT x 
Parameters: 
f    is a format identifier 
k    is an input/output list specifying the data to be transferred 
*    specifies that the operation is list-directed 
x    is a NAMELIST name. 
00001
If a formatted record is printed (i.e., output to a printing device such as a 
terminal or line printer), the first character of the record is not printed. 
The remaining characters of the record, if any, are printed in one line 
beginning at the left margin. 
The first character of a formatted record that is to be printed determines 
vertical spacing as follows: 
    Character      Vertical Spacing Before Printing 
    ---------      -------------------------------- 
    Blank          One line 
    0              Two lines 
    1              Skip to first line of next page 
    +              No advance (This option is available only on some 
                   line printers.) 
  All other characters in column 1 are treated as blanks.  Subsequent to 
  printing, the printer is not advanced.  If there are no characters in the 
  record, the vertical spacing is one line and no characters (other than 
  blanks) are printed in that line. 
READ_LIST_DIRECTED
Print positioning, scaling, and editing format specifications include the 
following: 
    S (sign control) 
    B (blank scan control) 
    Alternate Forms for Hollerith (',",$) 
    X (skip, space, or backspace) 
    T (tab) 
    TR (relative tabbing) 
    TL (relative tabbing) 
    P (scale factor power) 
    Slash (/) (record separator) 
    Colon specifier (:) 
For a list of related subtopics, enter: 
HELP (FORTRAN) PRINT_POSITIONING TOPICS 
See also the following FORTRAN HELP topics: 
    PARENTHESIZED_FORMAT 
    ADJUSTABLE_FORMATS 
    FORMATS 
S_FORMAT 
The form of the specifier is 
SP    or    SS    or    S 
This specifier is used to control the optional plus signs in numeric output. 
These are normally not produced by FORTRAN-77. 
The form 
SP 
is used to produce all otherwise optional plus signs in numeric output fields. 
(The plus signs, no longer optional, have become required in the current 
output statement.) 
The form 
SS 
is used to inhibit the appearance of the otherwise optional plus signs in 
numeric output. 
The form 
S 
is used to restore the status of optional plus signs to optional. 
B_FORMAT 
The form of the specifier is 
BN    or    BZ 
This specifier is used to determine how blanks are to be treated when scanning 
numeric input values with field width specified.  It has no effect on 
widthless input. 
The form 
BN 
is used to cause the processor to ignore blanks when processing numeric input 
with field width specified.  Blanks are counted as being part of the numeric 
input field width. 
The form 
BZ 
is used to cause the processor to treat blanks as being zeros when processing 
numeric input with field width specified.  Blanks are counted as being part of 
the numeric input field width. 
Whenever the BN or BZ format specification is invoked, the condition set by 
the specification remains in effect until the execution of the input/output 
statement is terminated or until another BZ or BN specification is encountered 
which establishes the current condition. 
Note that the default for an external or internal file is BZ unless the unit 
was OPENed with the BLANK='NULL' option specified. 
HOLLERITH_ALTERNATE_FORMATS 
These are ' " or $.  These are alternate formats for Hollerith transmission 
that have the advantage of not requiring the characters in the string to be 
counted. 
The form of the specifier is one of the following: 
"s" 
's' 
$s$ 
The string s may contain not more than 511 characters.  Any characters in the 
FORTRAN character set may appear; however, note the restriction below 
concerning the ", ', and $ characters themselves.  A repeat count, r, may 
optionally precede any of the three specifications. 
Output 
HOLLERITH_OUTPUT_ALTER 
The string s is transmitted to the external device in a manner similar to that 
for H format.  Thus, 
    'ABLE',$bBODIEDb$,"SEAMEN" 
is output as the string 
    ABLE BODIED SEAMEN 
Within a string enclosed by apostrophes, quotemarks or dollar signs, these 
characters themselves are represented by a double apostrophe, double dollar 
signs, or double quotemarks, respectively.  For example, within a string 
enclosed by apostrophes, two consecutive characters are interpreted as one ' 
character. 
Example: 
    '''TWAS BRILLIG AND THE SLITHY TOVES...' 
which would print as 
    'TWAS BRILLIG AND THE SLITHY TOVES... 
Within a ' string, a $ or " character has no special significance and the same 
is true for " strings and $ strings with regard to the other two string 
delimiters.  For example, the specifications 
    $I'LLb$, 'TAKE $' 
produce the external string 
    I'LL TAKE $ 
Input 
HOLLERITH_INPUT_ALTER 
The characters appearing between the apostrophes or dollar signs are replaced 
by the same number of characters, taken sequentially from the input string. 
Therefore, if the specification 
    'VECTOR' 
is used to process the input field 
    MATRIX 
the specification itself is changed to 
    'MATRIX' 
With one exception, blanks in FORMAT statements are significant only in H, ', 
", and $ specifications.  The one exception is that two apostrophes, two 
quotemarks, or two dollar signs are not considered consecutive if they are 
separated by a blank.  Therefore, the two FORMAT statements 
    1 FORMAT ($ABC$ $DEF$) 
    2 FORMAT ($ABC$$DEF$) 
are not equivalent.  They produce the following external strings, 
respectively: 
    ABCDEF 
    ABC$DEF 
This is the only exception to the rule that, with a FORTRAN program, blanks 
are significant only within H, ', ", and $ strings; in FORMAT statements, the 
blanks between the dollar signs (or apostrophes or quotation marks) are not 
within the string, but they cannot be removed without changing the meaning of 
the statement. 
X_SPEC 
The form is 
iX 
This specification causes no conversions to occur.  Instead, it causes i 
positions of the external field to be "skipped".  If i is positive, it has an 
effect similar to that of a space bar on a typewriter; if it is negative, it 
has an effect similar to that of the backspace control on a typewriter.  In 
particular, an attempt to backspace beyond the beginning of a record is 
equivalent to backspacing to the beginning of the record. 
Output 
OUTPUT_X_SPEC 
For positive values of i, the next i positions in the output record will be 
blanks (normally; however, see below).  In other words, a field of i blanks 
will be created.  For example, the specifications 
    'WXYZ',4X,'IJKL' 
generate the following external string: 
    XXYZbbbbIJKL 
 The X format specifier does not itself cause the extension of the output 
record.  It causes the moving of the print location; blanks are inserted only 
if other information follows the X specifier in the record.  Otherwise, if the 
X format specifier is the last or only specifier in the record, blanks are not 
put into the output stream. 
For example, for the code 
    WRITE (1,10) 
 10 FORMAT (1X) 
the result is that a record with zero bytes is written, not a record with a 
single blank. 
A negative value of i causes processing to "back up" in the record. 
    'FORTRAN' , -3X , 'KNOX' 
are equivalent to the specification 
    $FORTKNOX$ 
Note that when either backing up or moving forward by means of an X 
specification, characters that may have been previously produced in the 
positions being skipped are not destroyed.  Thus, in the example given above 
under X output, it is not necessarily true that the specification 4X produces 
four blanks.  It will, however, if no other characters have been generated in 
those positions, since all output records are initially set to blanks. 
The ability to specify a negative count in an X specification makes it 
possible to backspace over the blank that is produced at the end of external 
fields by widthless numeric formats (i.e., D,E,F,G, and I).  For example, for 
K = 13 and Q(13) = 350.8, the statements 
    PRINT 5,K,Q(K) 
  5 FORMAT( 'Q(' , I,-X,')=',F.2) 
generate the string 
    Q(13)=350.80 
As illustrated in the above example, if i is not specified it is assumed to be 
1.  Thus, the following specifications are equivalent: 
    XXXXX 
    5X 
Input 
INPUT_X_SPEC 
The next i characters from the input string are ignored whenever i is positive 
(that is, they are skipped).  For example, with the specifications 
    F5.3,6X,I3 
and the input string 
    76.41IGNORE697 
the characters 
    IGNORE 
are not processed. 
Negative values of i cause |i| characters from the input string to be 
processed again.  Consequently, the specifications 
    I3,-1X,E4.1 
and the string 
    123456 
are equivalent to 
    I3,E4.1 
and the string 
    1233456 
T_SPEC 
The form is 
Tw 
This specification causes processing (either input or output) to begin at 
character position w in the record, regardless of the position in the record 
that was being processed before the T specification. 
The T specification functions exactly like an X specification, i.e., no 
transfer of data occurs.  For example, the following FORMATs are equivalent: 
    1 FORMAT(5X,A8,-2X,I7) 
    2 FORMAT(T6,A8,T12,I7) 
It can be seen from the above example that it is permissible to tab either 
forward or backward.  Furthermore, a T specification provides a capability 
that an X specification does not, namely that of tabbing to a given print 
position when widthless formats are being used and the character position is 
thus unknown.  For example, to print (or read) three columns of numbers 
beginning in positions 1, 21, and 41, the following FORMAT statement could be 
used: 
    3 FORMAT(G.7,T21,G.7,T41,G.7) 
Note that backward tabbing can cause previously output information to be 
overwritten, or previously read input to be processed again. 
As with X specifications, it is not possible to tab to a position previous to 
the beginning of the record. 
If no w is given, it is assumed to be 1.  That is, T is the same as T1. 
TR_SPEC TL_SPEC 
The forms of the TR and TL specifications are 
TRi     TLi 
These specifications cause processing to begin i positions to the right (TR) 
or left (TL) of the current position in the record. 
These specifications function exactly like an X or T specification; that is, 
no conversions occur.  For example, the following FORMATs are equivalent: 
    1 FORMAT (5X,A8,-2X,I7) 
    2 FORMAT (T6,A8,T12,I7) 
    3 FORMAT (TR5,A8,TL2,I7) 
Note that tabbing to the left (TL) can cause previously output information to 
be overwritten, or previously read input to be processed again. 
As with X and T specifications, it is not possible to tab to a position 
previous to the beginning of the record. 
If no i is given, it is assumed to be 1.  TR is the same as TR1, and TL is the 
same as TL1. 
Note that TRi is the same as iX, and TLi is the same as -iX.  Refer to the 
X_SPEC for examples of Output and Input processing. 
P_SPEC 
The form is 
iP 
A P specification causes the value of the scale factor to be set to i, where 
the scale factor is treated as a multiplier of the forms 
    10**i for output 
and 
    10**(-i) for input 
At the beginning of each formatted input/output operation, before any 
processing occurs, the scale factor is set to zero.  Any number of P 
specifications may be present in a FORMAT statement, thereby causing the value 
of the scale factor to be changed several times during a formatted 
input/output operation.  If a FORMAT is rescanned within a single input/output 
operation due to the number of items in a list (see LIST_INTERFACE under 
PRINT_POSITIONING, the value of the scale factor is not reset to zero. 
Scale factors are effective only with F,E, and D conversions, floating-point 
input G conversions, and E-type output G conversions. 
Output (P Specifier) 
OUTPUT_P_SPEC 
The value of the list item is scaled by the multiplier 10**i.  This scaling 
causes the decimal point to be shifted right i places.  On D- and E-type 
conversions, the exponent field (+ee) is correspondingly reduced by i.  Thus, 
for D- and E-type output, the external number is equal to the internal value 
(except for rounding), while for F format output it is not (unless i is 0).  A 
scale factor does not affect a number whose value is zero.  The following 
examples illustrate output scaling: 
Format                      External field when internal value is: 
------                      -------------------------------------- 
                      2.71828         -2.71828      0.00000       0.09999 
                      -------         --------      -------       -------- 
 2PF10.3            271.828         -271.828         .000         9.999 
 1PF10.3             27.183          -27.183         .000         1.000 
 0PF10.3              2.718           -2.718         .000          .100 
-1PF10.3               .272            -.272         .000          .010 
-2PF10.3               .027            -.027         .000          .001 
 2PE14.3             27.18E-01       -27.18E-01      .000E+00    99.99E-03 
 1PE14.3              2.718E+00       -2.718E+00     .000E+00     9.999E-02 
 0PE14.3               .272E+01        -.272E+01     .000E+00      .100E+00 
-1PE14.3               .027E+02        -.027E+02     .000E+00      .010E+01 
-2PE14.3               .003E+03        -.003E+03     .000E+00      .001E+02 
The examples for E conversion above are similar to those that result from D 
conversion and E-type G conversion.  When G conversion uses the F form, 
however, scale factors do not apply.  Thus, a number output in G format always 
represents the internal value. 
Note that when a scale factor is in effect, output rounding takes place after 
the scaling has been performed.  In the case of E format, this may cause 
additional scaling to be required, as shown above in the output of 0.09999. 
Note the discontinuity in the way the exponent changes. 
Input (P Specifier) 
INPUT_P_SPEC 
During F,E,D, and G input conversions, if the input string contains an 
exponent field, the scale factor has no effect.  However, when the input 
string does not contain an exponent field, the value of the external field is 
scaled by 10**i; that is, the decimal point is moved left i places.  The 
following examples indicate the effect of scaling during an input operation: 
    External Field          Scale Factor        Effective Value 
    --------------          ------------        --------------- 
      -71.436                   0P                 -71.436 
                                3P                 -.071436 
                               -1P                 -714.36 
      -71.436E 00               3P                 -71.436 
                               -1P                 -71.436 
Note that, on both input and output, if the external number has an exponent 
specified, it is equal to the internal value; if it does not, then 
    external value=internal value x 10**i 
Once a scale factor has been established during an input/output operation, it 
remains in effect throughout the operation, unless redefined by an additional 
P specification.  To reset the scale factor to zero, it is necessary to write 
a OP specification.  For the list 
    A,K,X,B 
and the FORMAT 
    FORMAT(2(F.3,2P),E12.4,-2P) 
A, K, and B are all converted using the F.3 format specification, but all 
three have different scale factors in effect, as illustrated below: 
  List Item                Effective Format Specification 
  ---------                ------------------------------ 
     A                                 F.3 
     K                                2PF.3 
     X                               2PE12.4 
     B                                -2PF.3 
When i is not specified, its value is assumed to be zero.  Therefore, 
    P is equivalent to 0P 
SLASH_FORMAT_SPECIFIER 
The form may be 
r/  or  / 
Each slash (/) specified causes another record to be processed.  In the case 
of contiguous slash specifications (i.e., //// .../ or r/), since no 
conversion occurs between each of the slash specifications, records are 
ignored during input, and blank records are generated during output 
operations.  The same condition can occur when a slash specification and 
either of the parenthesis characters surrounding the field specifications are 
contiguous; a slash preceding the final right parenthesis in a FORMAT 
statement is not ignored. 
Output (Slash Specifier) 
OUTPUT_SLASH_SPECIFIER 
Whenever a slash specification is encountered, the current record being 
processed is output, and another record is begun.  If no conversion has been 
performed when the slash is encountered, a blank record is created. The 
statements 
       WRITE (5,10) X,Y 
    10 FORMAT (F5.3//I13) 
are processed in the following manner: 
1.  A record is begun, and X is converted with the specification F5.3. 
2.  The first slash is encountered, the record containing the external 
    representation of X is terminated, and another record is begun. 
3.  The second slash is encountered, the second record is terminated, and a 
    third record is started.  Note that since no conversion occurred between 
    the terminations of the first and second records, the second record was 
    blank. 
4.  The value of the variable Y is converted with the I13 specification, the 
    closing right parenthesis character is encountered, and the third record 
    is terminated. 
If a third item Z is added to the output list, as in 
    WRITE (5,10)X,Y,Z 
the following additional steps occur: 
5.  A fourth record is begun, and Z is converted using the specification F5.3. 
6.  The first slash is reencountered, the fourth record is terminated, and a 
    fifth record is begun. 
7.  Again, the second slash is processed; the fifth record, which is blank, is 
    terminated; and the sixth record is started. 
8.  Since there are no more list items, the specification I13 is not 
    processed, a termination occurs, and the final or sixth record, which is 
    also blank, is output. 
Note that the processing of Z in steps 5 through 8 is equivalent to processing 
with the statement 
    10 FORMAT(F5.3,//) 
since the specification I13 is not utilized. 
The original FORMAT statement can also be written as 
    10 FORMAT(F5.3,2/I13) 
or 
    10 FORMAT(F5.3,2/,I13) 
both of which cause identical effects. 
The two statements 
       WRITE (M,4)X 
     4 FORMAT(3/E.4/) 
cause the generation of three blank records, followed by a record containing 
the value of X (converted by the specification E.4), followed by another blank 
record. 
Input (Slash Specifier) 
INPUT_SLASH_SPECIFIER 
The effect of slash specifications during input operations is similar to the 
effect for output, except that for input, records are ignored in the cases 
where blank records are created during output.  For example, the statements 
    READ (M,4)X 
  4 FORMAT(3/E.4/) 
cause three records to be bypassed, a value from the fourth record to be 
converted (with the specification E.4) and assigned to X, and a fifth record 
to be bypassed.  This means that, as with the last example for output, records 
created with a FORMAT statement containing slash specifications can be input 
by use of the identical FORMAT statement, which is not true in FORTRAN systems 
that ignore a final slash. 
COLON_FORMAT_SPECIFIER 
The form is 
: 
The colon does not specify conversion of any list item.  Rather it causes the 
FORMAT processor to text for remaining list items. 
The colon (:) specifier may appear wherever a list separator (comma or slash) 
may appear. 
If any list items are remaining, FORMAT processing continues.  If there are no 
more list items, the current input/output in process is terminated. 
LIST_INTERFACE 
Formatted input/output operations are controlled by the FORMAT requested by 
each READ or WRITE statement.  Each time a formatted READ or WRITE statement 
is executed, control is passed to the FORMAT processor.  The FORMAT processor 
operates in the following manner: 
1.  When control is initially received, a new input record is read, or 
    construction of a new output record is begun. 
2.  Subsequent records are started only after a slash specification has been 
    processed (and the preceding record has been terminated) or the final 
    right parenthesis of the FORMAT has been encountered.  Attempting to read 
    (or write) more characters on a record than are (or can be) physically 
    present does not cause a new record to be begun. 
3.  During an input operation, processing of an input record is terminated 
    whenever a slash specification or the final right parenthesis of the 
    FORMAT is encountered, or when the FORMAT processor requests an item from 
    the list and no list items remain to be processed.  Construction of an 
    output record terminates, and the record is written on occurrence of the 
    same conditions. 
4.  Every time a conversion specification (i.e., F,E,D,G,I,L,A,R,Z,M, or N 
    specification) is to be processed, the FORMAT processor requests a list 
    item.  If one or more items remain in the list, the processor performs the 
    appropriate conversion and proceeds with the next field specification. 
    (If conversion is not possible because of a conflict between a 
    specification and a data type, an error occurs.)  If the next 
    specification is one that does not require a list item (i.e., H,$,',X,T,P, 
    or/), it is processed whether or not another list item exists.  Thus, for 
    example, the statements 
           WRITE (6, 12) 
        12 FORMAT(///4HABCD) 
    will produce three blank records and one record containing ABCD before 
    reaching the final right parenthesis.  When there are no more items 
    remaining in the list and the final right parenthesis is reached or a 
    conversion specification is found, the current record is terminated, and 
    control is passed to the statement following the READ or WRITE statement 
    that initiated the input/output operation. 
5.  When the final right parenthesis of a FORMAT statement is encountered by 
    the FORMAT processor, a test is made to determine if all list items 
    (including those to be used as values of N in adjustable specifications) 
    have been processed.  If the list is exhausted, the current record is 
    terminated, and control is passed to the statement following the READ or 
    WRITE statement that initiated the input/output operation.  However, if 
    another list item is present, an additional record is begun, and the 
    FORMAT statement is rescanned.  The rescan takes place as follows: 
    a.  If there are no parenthesized groups as specifications within the 
        FORMAT statement, the entire FORMAT is rescanned. 
    b.  If one or more parenthesized groups do appear, however, the rescan is 
        started with the group whose right parenthesis was the last one 
        encountered prior to the final right parenthesis of the FORMAT 
        statement.  In the following example, the rescan begins at the point 
        indicated: 
        FORMAT(3X,(F7.2,A5),(X'ABC'3(3I4,2(G15.7//),R3)),E20.12,3HXYZ) 
                          rescan             last closing        final right 
                          begins             parenthesis of      parenthesis 
                          here               internal group      of FORMAT 
    c.  If the group at which the rescan begins has a repeat count (r) in 
        front of it, the previous value of the repeat count is used again for 
        each rescan.  In particular, if the repeat count was specified with an 
        N, a new value of N is not supplied when the rescan takes place; the 
        old value is used.  Thus, for example, the statements: 
              PRINT 5,CODE,5,(A(J),J=1,50) 
            5 FORMAT(A4/N(G20.8)) 
          are equivalent to the statements 
              PRINT 5,CODE,(A(J),J=1,50) 
            5 FORMAT(A4/5(G20.8)) 
6.  When a colon FORMAT specifier is encountered by the FORMAT processor, a 
    test is made to determine if all the list items have been processed.  If 
    the list has not been exhausted, the FORMAT processor continues FORMAT and 
    list interfacing.  However, if the list has been exhausted, 
        -  The current record is terminated, and 
        -  Control is passed to the statement following the READ or WRITE 
           statement that was in process. 
7.  Each list item to be converted is processed by one specification or one 
    iteration of a repeated specification, with the exception of a complex 
    datum which is processed by two such specifications. 
8.  Each READ or WRITE statement containing a nonempty list must refer to a 
    FORMAT statement that contains at least one adjustable or conversion 
    specification (see step 4 above).  If this condition is not met, the 
    FORMAT statement will be processed, but an error will occur. 
9.  The same rules apply to DECODE and ENCODE operations as to READ and WRITE. 
    The interpretations of multiple records in these cases is described under 
    ENCODE_DECODE 
CARRIAGE_CONTROL_PRINT_OUTPUT 
The first character of an output record that is intended for printing is used 
to control the printer carriage by containing certain characters. 
    Character     Effect 
    ---------     ------ 
       1          Skip to first line of page before printing, do not space 
                  after printing. 
       0          Space two lines before printing, do not space after 
                  printing. 
       +          Do not space before printing, do not space 
                  after printing. 
     blank        Space one line before printing, do not space 
                  after printing. 
Any other character appearing as the first character in a record causes the 
carriage to be single-spaced before the record is printed.  The first 
character of the record is not printed.  The second character of the output 
record is aligned to the left margin. 
See additional library subprogram VFCP6 (under COMMON_LIB) for access to CP-6 
Monitor Device Format Control characters. 
00014
00002
00019
00018
00002
00003
00011
00001
00014
00009
00010
00010
00006
00003
00005
00004
00013
00016
00008
00012
00015
00007
00019
00011
00011
00017
00014
00014
00001
00010
00010
00010
00010
00009
00009
00006
00006
PROGRAM_STATEMENT
MAIN_PROGRAMS
EXECUTABLE_PROGRAM
Syntax: 
PROGRAM p 
Parameters: 
p    is the symbolic name of the program; it must not be the same as any other 
symbolic name in the program unit.  Also, p must not be the same as any other 
subprogram name, entry name, common block name, or global name in the 
executable program. 
Description: 
The PROGRAM statement is used to give a symbolic name to a main program, and 
may only appear as the first statement in a main program. 
00002
00001
Syntax: 
CALL PROMPT(u[,c[,v[,l[,e]]]]) 
Parameters: 
u    is an integer value (expression result) that specifies the unit for which 
the new prompt is to be used as performed by monitor services M$PROMPT (enter 
HELP (HOSTMON) M$PROMPT for details) If the unit number is the only argument 
that is specified, the prompt character is reset to the default FORTRAN 
run-time prompt, the '?'. 
c    is a character expression that may contain up to 94 characters (including 
vfc character).  This will be the character string that is used to prompt the 
user with each read on the specified unit number. 
v    is an integer flag.  If zero, vfc will not be used on the read; if other 
than zero, vfc will be used and the vfc character is the first character of 
the prompt string. 
l    is an integer to indicate the length of the prompt string. 
e    is an integer variable or array element.  The error code (if an error 
occurs) will be stored into this parameter if it is present.  Zero is returned 
if an error does not occur during the prompt operation.  If an error occurs 
and the parameter is not present, a run-time diagnostic will be generated. 
Description: 
The prompt string will be associated with the unit number.  The vfc (v, 
integer flag) will indicate whether the first character of the prompt string 
is a vfc character.  If the prompt character is not passed, the default prompt 
'?' will be associated with the unit number.  If the unit is not assigned to a 
terminal device or comgroup, the setting of the prompt will be ignored. 
Examples: 
    CALL PROMPT (10,'!',0,1,ERRCODE) 
    READ (10,*) I 
    !input 
00002
00003
00001
Syntax: 
PUNCH f [,k] 
    or 
PUNCH *,k 
    or 
PUNCH x 
Parameters: 
f    is a format identifier 
k    is an input/output list specifying the data to be transferred 
*    specifies that the operation is list-directed. 
x    is a NAMELIST name. 
00001
Syntax: 
{READ|WRITE} DISK u,s,k 
Parameters: 
u    is an integer, real or double precision expression whose integer value 
specifies the logical unit number of the disk. 
s    is also an integer, real, or double precision expression whose integer 
value defines the starting disk address. 
k    is an input/output list, as described previously. 
Description: 
Random access input/output operations are unformatted, and therefore do not 
reference a FORMAT statement.  However, they differ from the standard 
unformatted (binary) READ/WRITE statements in two ways: 
1.  They refer to random access files rather than to sequential files. 
    Consequently, the REWIND, BACKSPACE, and ENDFILE operations are not 
    applicable to them. 
2.  Information is not thought of as being broken into records.  Data is 
    processed exactly as specified, with no control words or record 
    boundaries.  As many locations of the disk are used as are required for 
    the items specified in the input/output list.  With a knowledge of the 
    required sizes of various items (see STORAGE_ALLOCATION_STATEMENTS), the 
    programmer is not bound by the binary READ/WRITE restriction that the data 
    written by one WRITE statement must be input with one and only one READ 
    statement. 
    As an analogy, the disk may be thought of as a one-dimensional array, from 
    which it is possible to select individual or groups of elements in any 
    random order, much as in an ENCODE or DECODE statement (see 
    ENCODE_DECODE). 
In WRITE DISK statements, the internal values of the list items are written on 
the appropriate device, starting at the location defined by s. 
The value of s may be considered to be either an absolute random access device 
address or may be an address relative to the start of the user's file. 
Interpretation of the address depends on the operating environment in which 
the program is being run. 
Note that a logical unit number used in standard READ/WRITE statements can be 
assigned to a sequential disk file.  In this case, the disk acts like a 
magnetic tape and the random access statements would not be used. 
00002
00001
Syntax: 
READ (specifier_list) [k] 
    or 
READ f [,k] 
Parameters: 
specifier_list    is a list of specifiers that includes 
    (1)  A unit specifier 
    (2)  An optional format specifier 
    (3)  An optional error specifier 
    (4)  An optional record specifier 
    (5)  An optional input/output status specifier 
f    is a format identifier 
k    is an input/output list specifying the data to be transferred 
The various forms of the READ statement are described under READ_FORMATTED, 
READ_UNFORMATTED, READ_LIST_DIRECTED, READ_END_ERR_IOSTAT, READ_KEYWORD, 
READ_ALTERNATE_DIRECT_ACCESS, and READ_NAMELIST. 
Each of these forms have differing requirements for the specifier_list and the 
input/output list. 
00001
READ (u'r[,f])list 
    or 
READ (u'r[,f],ERR=k)list 
Parameters: 
u    is a logical unit number.  It may be an integer, real, or double 
precision expression, truncated to integer. 
'    is a field separator required to distinguish the direct-access READ from 
other read statements. 
r    is an integer expression for the record number associated with the 
specified logical unit. 
f    is the optional format reference.  With f, the statement is formatted; 
without f, the statement is unformatted. 
ERR=k    is an optional error transfer specification, with k the label of the 
statement to which control is transferred in case an error condition is 
encountered during this READ. 
list    specifies an optional I/O list.  The list may not contain the 
associated variable for the logical unit defined in the DEFINE FILE statement. 
Description: 
The direct access READ statement is used in conjunction with the DEFINE FILE 
statement.  A FORMAT statement may be referenced in a direct access READ 
statement. 
Examples: 
     DEFINE FILE 10(700,100,L,IREC1),20(300,32,L,IREC2) 
     DIMENSION N(20) 
     . 
     . 
     . 
     IREC2=32 
     . 
     . 
     . 
  20 FORMAT(10I10) 
  11 READ (10'18,20)(N(I),I = 1,20) 
     . 
     . 
     . 
  15 READ (20'IREC2+3)X,Y,Z,S,T,U,V,W 
The statement with the label 11 transmits data from the file associated with 
logical unit 10.  Transfer is under control of the FORMAT with the label 20 
and begins with record 18.  Since the I/O list specifies 20 variables and the 
FORMAT statement specifies 10 variables per record, two records, 18 and 19, 
will be read.  Therefore, the associated variable, IREC1, is set to 20. 
The statement with the label 15 reads data from the file associated with 
logical unit 20, without format control, beginning with record 35.  Since the 
record size of 32 bytes is equal to the I/O list of eight real variables, only 
one record is read.  The associated variable IREC2 is set to 36 at the 
conclusion of the operation. 
The format specification L in the DEFINE FILE statement of the example could 
be changed to E for the file associated with logical unit 10, and to U for the 
file associated with logical unit 20, without affecting the result. 
As with normal READs (or WRITEs), the FORMAT statement may also control the 
point at which reading starts.  For example, if the FORMAT statement with the 
label 20 of the example had been written as: 
    20 FORMAT(//10I10) 
records 18 and 19 would be skipped, record 20 read, records 21 and 22 skipped, 
and record 23 read.  IREC1 in this case would be set to 24 at the end of the 
READ operation at statement 11. 
00002
00003
00001
WRITE_DIRECT
RANDOM_WORD_ACCESS_STATEMENTS
Syntax: 
     For Sequential READ 
     ------------------- 
     READ(u,f,[END=k1],[ERR=k2],IOSTAT=ios)list 
     READ(u,[END=k1],[ERR=k2],IOSTAT=ios)list 
     READ(u,*,[END=k1],[ERR=k2],IOSTAT=ios)list 
     For Direct-Access READ 
     ---------------------- 
     READ(u,f,REC=n,[ERR=k2],IOSTAT=ios)list 
     READ(u,REC=n,[ERR=k2],IOSTAT=ios)list 
     For Keyed-Access READ 
     --------------------- 
     READ(u,f,KEY=z,[ERR=k2],IOSTAT=ios)list 
     READ(u,KEY=z,[ERR=k2],IOSTAT=ios)list 
Parameters: 
u    is a logical unit number.  It may be an integer, real, or double 
precision expression, truncated to integer if necessary. 
*    specifies that the operation is list-directed. 
f    is a format reference. 
n    is an integer,real, or double precision expression that has a positive 
value after truncation to integer.  It denotes the record number. 
z    denotes the key value. 
END=k1    k1 is a statement label.  The END= parameter is permitted only in a 
sequential READ statement. 
ERR=k2    k2 is a statement label. 
ios    is an integer variable or integer array element. 
Both END= and ERR= are optional; if both are present, either may appear first. 
Description: 
The formatted, unformatted, and list-directed READ statements may optionally 
include the END=, ERR=, and IOS= parameters.  END=k1 specifies immediate 
transfer of control to statement k1 if an end-of-file mark is encountered 
during the processing of a READ statement.  ERR=k2 specifies immediate 
transfer of control to statement k2 if an error occurs during the processing 
of a READ statement.  Execution of an input/output statement containing the 
ios specifier causes ios to be set as shown below. 
Value of ios             Condition 
-----------              --------- 
Zero            Neither an error condition nor an end-of file 
                condition is encountered by the processor. 
Positive        An error condition is encountered; ios is set 
Integer         to a positive integer value (as returned in 
Value           the Monitor Services ALTRET frame). 
Negative        An end-of-file condition is encountered and 
Integer         no error condition is encountered. 
Value (for 
READ only) 
00002
00001
Syntax: 
The simplified formatted READ statements are written as 
  Sequential        Direct Access          Keyed Access 
  ----------        -------------          ------------- 
  READ(u,f)[k]      READ(u,f,REC=n)[k]     READ(u,f,KEY=z)[k] 
See READ_END_ERR_IOSTAT for additional formatted READ statement forms, 
and also  READ_ALTERNATE_DIRECT_ACCESS. 
Parameters: 
u    is a logical unit number.  It may be any integer, real, or double 
precision expression, truncated to integer if necessary.  In the sequential 
case, it may also be a character variable, a character array name, or a 
character array element; in which case the character entity becomes the 
storage file and the READ statement is interpreted as a storage file I/O 
statement. 
f    is a format reference.  It may be the statement label of a FORMAT 
statement, a variable that has had such a statement label assigned to it, the 
name of an array of a type other than character in which a format has been 
stored (see FORMATS_STORED_IN_ARRAYS under the topic FORMATS) or a character 
variable, a character array, or a character expression. 
k    is an optional input/output list. 
n    is an integer, real, or double precision expression that has a positive 
value after truncation to integer.  It denotes the record number. 
z    denotes the key value and is a character expression or an integer, real, 
or double precision expression, truncated to integer.  If a character 
expression, trailing blanks are truncated and are not included in the key 
value used in the I/O. 
Description: 
Formatted READ statements are used to process formatted records.  The form of 
conversion to be performed between the internal data and the external fields 
is specified by a FORMAT statement.  There are three kinds of formatted READ 
statements:  sequential, direct, and keyed access. 
A formatted READ statement causes the character string in the external record 
to be converted into internal values according to the FORMAT specified.  The 
converted values are then assigned to the variables appearing in the list k 
(or the equivalent simple list, if k contains a DO implication). 
Each formatted READ statement begins processing with a new record.  Thus, it 
is not possible to process a particular record using more than one READ 
statement.  (This can be done with the DECODE and ENCODE statements.)  More 
than one record may be processed by these statements if specifically requested 
by the FORMAT statement.  However, attempting to read more characters on a 
record than are (or can be) physically present does not cause a new record to 
be begun. 
The maximum size of a formatted record depends on the record size for the 
logical unit number.  Certain devices, e.g., card punch and card reader, 
impose absolute restrictions on the size of records:  a punched card contains 
80 characters.  If a record contains no characters, it is considered to be 
blank (or empty); in other words, a record into which any number of blanks 
have been specifically written is indistinguishable within the program from an 
empty record.  Note, however, that on devices such as magnetic tape or paper 
tape, the FORMAT statement may determine the actual size of the record 
written. 
The maximum size of a formatted record for disk files is 133 characters in 
length.  This maximum, which is the default, can be overridden by use of the 
RECL=rl option of the FORTRAN OPEN statement. 
The list k may be omitted from a formatted READ statement.  Normally, this has 
the effect of skipping one record on input.  However if the FORMAT statement 
begins with Hollerith or slash specifications (see LIST_INTERFACE under the 
topic PRINT_POSITIONING) information may actually be processed and one or more 
records be used. 
Examples: 
Sequential formatted READ statements: 
     READ(105,6)X,Y,GRUNCH,T(3,5) 
     READ(5,FORM),(A(I),I=1,40),H,Q 
Direct access formatted READ statements: 
     READ(10,7,REC=9)X,A,B,C 
     READ(21,'(F10.3,416)',REC=7)A,I 
Keyed access formatted READ statements: 
     READ(10,7,KEY=91)X,A,B,C 
     READ(21,'(F10.3,416)',KEY='ABLE10')A,J,K,L,M 
00002
00003
00001
INTERNAL_FILE_READ
Syntax: 
READ(UNIT=u[,FMT=f][REC=n|KEY=z][,END=k1][,ERR=k2][,IOSTAT=ios])[list] 
Parameters: 
u    is the logical unit number.  May appear in any order. 
f    is the format specifier. 
n    is an integer, real, or double precision expression that has a positive 
value after truncation to integer.  It denotes the record number. 
z    denotes the key value and is a character expression or an integer, real, 
or double precision expression, truncated to integer.  if a character 
expression, trailing blanks are truncated and are not included in the key 
value used in the I/O. 
k1    is a statement label.  If an end-of-file mark is encountered during the 
processing of the READ statement, control will be transferred immediately to 
the statement with the label k1. 
k2    is a statement label.  If an error is encountered during the processing 
of the READ statement, control will be transferred immediately to the 
statement with the label k2. 
ios    is an integer variable or integer array element.  Execution of a READ 
statement containing this specifier causes ios to become to a value as shown 
in VALUES_OF_IOS. 
list    is an input/output list (see IO_LISTS). 
Description: 
Keyword forms are alternate forms of the READ and WRITE statement.  The 
keyword form for unit number is UNIT=u, where u is the logical unit number, 
and FMT=f for the format specifier where f is the format specifier. 
If both the unit number and format specifier appear in a READ statement and 
the keyword form UNIT=u is used, the keyword form FMT=f must also be used. 
FMT=f and UNIT=u specifier may appear in any order among the other specifiers 
(e.g., the ERR= and, if present, the END=, REC= or the KEY=). 
Examples: 
     Formatted Sequential READs 
     -------------------------- 
           READ(UNIT=10,FMT=20,ERR=99,END=90,IOSTAT=INT)A,B 
           READ(ERR=901,UNIT=7,END=70,FMT=921)Z1,Z2 
           READ(FMT=21,END=999,ERR=707,UNIT=I)A,B,C,D 
     Formatted Direct-Access READs 
     ----------------------------- 
           READ(UNIT=9,FMT=12,REC=J,IOSTAT=I(J),ERR=99)X,Y,Z,A 
           READ(REC=J*1000,ERR=99,FMT='(I,NA)',UNIT=L)I,I,C 
           READ(FMT=ARRAY,UNIT=U1,REC=IU1)J,K,L,M,N 
     Formatted Keyed-Access READs 
     ---------------------------- 
           READ(UNIT=21,FMT=101,KEY=1000)A(K),A(K+1) 
           READ(KEY='ABCDE',UNIT=A,FMT=B)W1,W2,W3 
           READ(ERR=11,KEY=I*1000+J,UNIT=1,FMT=7)U1,U2 
     Unformatted Sequential READs 
     ----------------------------- 
           READ(UNIT=1,END=2,ERR=1)A,X1,OM,HIGH 
           READ(END=7,IOSTAT=J,UNIT=1,ERR=1)Z,Z2,X1,X(I) 
           READ(ERR=11,END=7,UNIT=1)W,LAMBDA,ZETA 
     Unformatted Direct-Access READs 
     ------------------------------- 
           READ(UNIT=2,REC=7,ERR=11)W,X 
           READ(REC=8,UNIT=2,ERR=12)Y,Z 
     Unformatted Keyed-Access READs 
     ------------------------------ 
           READ(UNIT=4,KEY=2000,ERR=12)X,Y 
           READ(KEY='XYZ',UNIT=5,ERR=11)X1,X2,Z 
     List Directed Sequential READs 
     ------------------------------ 
           READ(UNIT=7.FMT=*,ERR=71)A,B,C,D,E 
           READ(FMT=*,UNIT=71,END=747)X,Y,Y2,Z1 
00002
00003
00001
READ(u,*)k 
READ *,k 
and 
WRITE(u,*)k 
PRINT *,k 
u    is a logical unit number.  It may be an integer, real, or double 
precision expression, truncated to integer, if necessary. 
*    specifies that the operation is list-directed. 
k    is an I/O list.  If omitted, nothing will be read or written by the I/O 
operation. 
These statements process formatted records.  The format is implied by the data 
type of each item in the I/O list.  (For input formats, see INPUT_WITH_LIST.) 
The simplest forms are those shown above. 
00002
00001
NAMELIST_READ
READ
Syntax: 
The simplest forms are: 
  Sequential     Direct Access        Keyed Access 
  ----------     -------------        ------------ 
  READ(u)[k]     READ(u,REC=n)[k]     READ(u,KEY=z)[k] 
See also READ_END_ERR_IOSTAT for additional forms of the 
unformatted READ statement. 
Parameters: 
u    is a logical unit number.  It may be any integer, real, or double 
precision expression, truncated to integer if necessary. 
n    is an integer, real, or double precision expression. 
k    is an input/output list, which may be omitted. 
z    is a character expression, or an integer, real or double precision 
expression.  If a character expression, trailing blanks are truncated. 
A comma preceding the list k is optional. 
Description: 
The unformatted READ statements process information in internal (binary) form 
and are designed to provide temporary storage on magnetic tapes and disks. 
The three kinds of unformatted READ statements are sequential, direct access, 
and keyed access. 
The unformatted READ statements process data as a string of internal values 
arranged into words depending on the size of the items in the list k (see 
VARIABLE_ALLOCATION).  All items appearing in the list of a binary READ 
statement are contained in one logical record. 
The information output by a single unformatted WRITE statement must be input 
by one and only one unformatted READ statement.  It is permissible to read 
less information that is present in the record, but an error will occur if the 
input list requests more data than is present in a record.  The number of 
items that can be processed by a single sequential unformatted READ/WRITE 
statement is unlimited, but only one logical record will be read or written 
regardless of the amount of data to be transferred.  The number of items that 
can be processed by a single direct-access unformatted READ/WRITE is limited 
by the record size declared for the file. 
If the list k is omitted from an unformatted READ/WRITE statement, a record is 
skipped, or an empty record is written.  Unlike formatted input/output 
statements, such an operation can cause no data transfer.  Note that if an 
empty record is written, it can only be processed by a READ statement with no 
list and, therefore, has little purpose. 
Examples: 
Unformatted READ statements: 
     Sequential 
     ---------- 
     READ(3)(E1(K),(M(K,L),L=1,22),K=KLOW,KHIGH) 
     READ(N),ARRAY 
     Direct Access 
     ------------- 
     READ(7,REC=K+1)ARRAY 
     READ(M,REC=21),(ARRAY(J),J=K,L) 
     Keyed Access 
     ------------ 
     READ(7,KEY=K+1)ARRAY 
     READ(M,KEY=21),(ARRAY(J),J=K,L) 
00002
00003
00001
A record is a sequence of values or a sequence of characters.  A record does 
not necessarily correspond to a physical entity, but sometimes does.  There 
are five forms of records: 
UNFORMATTED 
FORMATTED 
ENDFILE 
FREE_FORM 
SELF_IDENTIFIED 
A property common to all forms of records is that an input statement may not 
specify that more data be read from a record than the record contains. 
To display a particular record (such as Unformatted), enter: 
HELP (FORTRAN) RECORDS UNFORMATTED 
To display all available record types enter ??. 
An unformatted record consists of a sequence of values in internal form.  The 
length of an unformatted record is measured in words.  The only statements 
that read and write unformatted records are unformatted input/output 
statements. 
A formatted record consists of a sequence of characters in the FORTRAN 
character set; its length is measured in characters.  The only statements that 
read or write formatted records are formatted input/output statements and 
list-directed input/output statements.  The maximum record size for formatted 
records is 133 characters in length.  This maximum, which is the default, can 
be overridden by use of the RECL=rl option of the FORTRAN OPEN statement. 
An endfile record is written by an ENDFILE statement.  An endfile record does 
not have length. 
A free-form record consists of the number of physical records required for an 
INPUT statement.  Each physical record contains constant values and 
separators.  The number of values in the record is unknown to the run time 
processor.  The only statements that read free-form records are free-form 
input statements.  Each free-form input statement begins by reading a new 
physical record.  There is no free-form output statement.  Note that the run 
time processor cannot distinguish between a free-form record and a formatted 
record.  (See INPUT_STATEMENT.) 
A self-identified record contains a series of "assignments" that are 
interpreted as assigning constant values to the variables and arrays appearing 
in the assignments.  Self-identified records may only be processed by 
self-identified input/output statements. 
00003
00002
00004
00004
00005
00001
When a dummy argument corresponds to an actual argument in the calling 
argument list, a reference to the dummy argument is a reference to the actual 
argument.  Thus, not only does the dummy argument initially have the value 
assigned to the actual argument at the time of the call, but any value 
subsequently assigned to the dummy will be assigned to the actual argument, 
thus effectively returning a result through the argument list.  (In 
FORTRAN-77, arguments are passed by location or reference; thus, this 
statement is always true.  Some other FORTRAN systems use call-by-value, 
introducing restrictions on assignments of this kind.)  For example, 
    .                                    FUNCTION SQRTDS(A,B) 
    .                                    C=AMAX(A,B) 
    .                                    B=AMIN(A,B) 
    Y = X ** NI + SQRTDS (Z,Q)           A=AMAX(A,C) 
    .                                    SQRTDS=SQRT(A**2-B**2) 
    .                                    RETURN 
    .                                    END 
where the values of Z and Q will be reversed whenever the initial value of Q 
is greater than that of Z.  (Functions that perform assignments of this kind 
are said to have "side effects" and be abnormal.) 
On the other hand, when a dummy argument corresponds to an expression, the 
expression serves merely to initialize the value of the dummy, and 
consequently the value of the dummy argument should not be changed within the 
subprogram.  Note that a single constant is a form of expression, as is a 
function reference (as opposed to a function name alone).  For example, if X 
is a variable and F is a function, 
    CALL ALPHA(X,4.6,F,F(X))        SUBROUTINE ALPHA(A,B,C,D) 
the dummy arguments B and D must not be assigned values within ALPHA, while 
this is permitted for the dummy argument A.  The dummy C must be used within 
the subprogram as a subprogram identifier. 
When a dummy argument corresponds to a statement label, the effect is the same 
as: 
    ASSIGN k TO d 
where k is the statement label appearing as the actual argument and d is the 
dummy argument.  Such a dummy argument may be used in a RETURN v statement. 
Note, however, that a statement label argument is much like a constant, and 
therefore the dummy argument should not be assigned a value. 
The maximum number of arguments associated with a CALL or function reference 
is 511. 
    A relational expression is of the form 
    e1 r e2 
    where 
    e1 and e2    are both arithmetic expressions or are both character 
    expressions. 
    r    is a relational operator 
    Evaluation of relational expressions results in either of the two values 
    "true" or "false" (a logical result). 
    Relational operators cause comparisons between expressions. 
        Operator       Meaning 
        --------       ------- 
        .LT. or <      < Less than 
        .LE.           < Less than or equal to 
        .EQ.           = Equal to 
        .NE.           = Not equal to 
        .GE.           > Greater than or equal to 
        .GT. or >      > Greater than 
A relational expression has the value "true" if the comparison is "true".  For 
example, 
    1  .LT.  6  is true 
    0  >  8   is false 
    When two arithmetic expressions are compared using a relational operator, 
    the two expressions are first evaluated, each in its own type, then the 
    comparison is made in the type of higher type precedence; i.e., the value 
    of the expression of the lower type precedence is converted to that of 
    higher precedence.  If either expression in a relational comparison is of 
    complex mode, the relational operator must be either 
       .EQ. or .NE. 
    In complex type comparisons, both real and imaginary parts are tested for 
    the appropriate relationship.  Double precision, real, and integer values 
    are assumed to have imaginary parts of zero value in complex mode 
    comparisons. 
Note that a test for equality between real or double precision quantities may 
not be meaningful on a binary machine.  Since these quantities are only 
approximations to most values, numbers that are "essentially" equal may differ 
by a small amount in their binary representations.  Therefore, only those 
computations whose operands and results have exact binary representations will 
produce accurate results for the relational operations .EQ. or .NE. 
00001
Syntax: 
REPEAT k, FOR v=e 
Parameters: 
k    is the statement label of the terminus of the REPEAT range. 
v    is a variable of integer, real, or double precision data type. 
e    is a repeat triple of the form 
    (e1,e2,e3) or (e1,e2) 
  where the ej are expressions of an appropriate type. 
The comma after k must appear as shown. 
Description: 
This is the second form of the REPEAT statement.  See REPEAT_WHILE 
description. 
Using the REPEAT Triple 
The REPEAT triple has a meaning similar to the ei in a DO statement; namely, 
it represents the set of values from e1 to e2 in steps of e3.  As in the DO 
statement, if e3 is omitted, its value is assumed to be 1. 
Unlike a DO loop, the loop created for a REPEAT triple is completely dynamic. 
This means that 
o   The control variable v can be modified within the loop, if desired. 
o   The increment and limit expressions (e3 and e2, respectively) are 
    evaluated at the beginning of each iteration; therefore, their values may 
    also be modified within the loop. 
o   If the termination condition is met initially, the loop is not executed at 
    all. 
o   It is permissible to transfer control into a REPEAT FOR loop without 
    having previously transferred control out. 
The execution of a REPEAT FOR triple proceeds in the following manner: 
1.  The variable v is assigned the value of expression e1 (the type is 
    converted if necessary). 
2.  The expression e3 is evaluated and converted to the type of v if 
    necessary. 
3.  The expression e2 is evaluated and converted to the type of v if 
    necessary. 
4.  If v is beyond e2 in the appropriate direction, control is transferred to 
    the statement following the terminus (i.e., to the statement following the 
    one whose label is k).  Specifically, if (e3>0 .AND. V>e2 .OR.  e3<0 .AND. 
    v<e2) the triple is terminated; otherwise, the process continues with step 
    5. 
5.  The range of statements controlled by the REPEAT statement is executed for 
    one iteration. 
6.  The expression e3 is evaluated and converted to the type of v if 
    necessary. 
7.  The variable v is incremented by the value of e3, and the process is 
    repeated from step 3. 
8.  The actual number of iterations defined by REPEAT triple depends on the 
    values of e3 and e2 during each of the iterations, as does the value of v 
    when the loop is exited, either in the normal way or by transferring 
    control out of it. 
In a real or double precision REPEAT loop, when any of the REPEAT-triple 
expressions (particularly e3) has a value that does not have an exact binary 
representation, it cannot be guaranteed that the value of v will ever exactly 
equal e2.  Thus, for example, the following REPEAT loop may be executed either 
11 or 10 times due to the approximate nature of real and double precision 
data. 
    REPEAT 47, FOR X=(1,2,.1) 
For integer data, the following REPEAT loop will be executed 11 times 
    REPEAT 48, FOR I=(10,20,1) 
Other examples of REPEAT FOR statements: 
    REPEAT 101, FOR N=(1,10) 
    REPEAT 6, FOR TRAN4=(SIN(X),COS(X),(1 -EXP(X/Y))*.014) 
    REPEAT 44, FOR FARADS=(1E-9,1E-7,1E-9) 
Nesting and REPEAT Ranges 
REPEAT statements may be nested in exactly the same manner as DO statements; 
in fact, a total nest may consist of both REPEAT statements and DO statements. 
The rules concerning the terminal statement of a REPEAT range are the same as 
for a DO range. 
For example: 
         REPEAT 112, WHILE Q=R .NE. T 
         . 
         . 
         . 
         DO 112 L=5,100,5 
         . 
         . 
         . 
         Z=A(L) 
         REPEAT 5, FOR TEST=(56,74,2) 
    5    Z=TEST**S+42.5*Y+Z 
         . 
         . 
         . 
    112  CONTINUE 
The various DO and REPEAT ranges may not overlap. 
All transfers into a REPEAT range are valid. 
The terminal statement of a REPEAT range may be any executable statement other 
than one of the following: 
    DO statement              RETURN statement     ELSE statement 
    REPEAT statement          STOP statement       ELSE IF statement 
    GO TO statement           END statement        END IF statement 
    Arithmetic IF statement   Block If statement 
00002
00004
00001
00003
REPEAT_FOR
Syntax: 
REPEAT k, WHILE e 
Parameters: 
k    is a statement label. 
e    is a logical expression. 
The comma must appear as shown. 
Description: 
This statement means:  For as long as the expression e has the value "true", 
repeat executing the range of statements beginning with the one following the 
REPEAT statement and ending with the one whose label is k. 
Since expression e is evaluated prior to each execution of the REPEAT range, 
the value of the expression may change during execution of the REPEAT range. 
If prior to any iteration the value of the expression e is "false", control is 
passed to the statement following the statement whose label is k. 
Consequently, if the initial value of the expression is "false", the range is 
not executed at all. 
Examples: 
    REPEAT 12, WHILE SUM<TEST 
    REPEAT 113, WHILE A .OR. B .AND. C 
    REPEAT 037, WHILE X**2+F(1,3,7,K,M,L(I,K)) .LE. -3.442 1E-4 
    REPEAT 2554, WHILE S/D/4+3>R .OR. N .EQ. J 
    REPEAT 88, WHILE LOGIC(A,Q(I),3HQED) 
    REPEAT 99, WHILE (A .AND. B .EOR. C) 
00002
00003
00001
REPEAT_WHILE
Syntax: 
RETURN [v] 
Parameters: 
v    is a variable or an array element (usually a dummy argument).  A second 
Please note that a second form of the RETURN statement is provided for 
compatibility with other FORTRAN systems.  (See RETURN_FORM2.) 
Description: 
The RETURN statement causes an exit from a subprogram.  A RETURN statement 
must be chronologically the last statement executed in any subprogram; it need 
not be physically the last.  There may be any number of RETURN statements in a 
subprogram.  In a main program a RETURN statement will be treated as a STOP 
statement. 
RETURN used without the optional v is the usual form.  In a subroutine, it 
returns control from the subroutine to the first executable statement 
following the CALL statement that called the subroutine.  In a function, it 
causes the latest value assigned to the function name to be returned as the 
function value to the expression in which the function reference appeared. 
(See also, FUNCTION.) 
RETURN v, with a variable or an array element specified, is used to provide an 
alternate exit from a subprogram.  The effect of this statement is the same as 
that of an assigned GO TO, namely: 
1.  The variable v must have a statement label assigned to it.  On RETURN 
    statements, the usual way to do this is for v to be a dummy variable and 
    to correspond to a statement label (or another assigned variable) in the 
    calling program.  (See ARGUMENTS_DUMMIES.)  When used as an argument, a 
    statement label is preceded by an asterisk, as in 
        CALL TEST3(X,A*B,*74) 
2.  Control is transferred immediately to the statement label to which v 
    corresponds. 
3.  If the subprogram is a function, no value is returned for the function and 
    the expression in which the function reference appears is not evaluated. 
Examples: 
    Calling Program                     Subprograms 
    33   CALL IT(LOCK,*11,*883)         SUBROUTINE IT(I,J,K) 
         .                              . 
         .                              . 
         .                              . 
         ASSIGN 55 TO LBL               RETURN J 
         .                              . 
         .                              . 
         .                              . 
    48   CALL IT(HOME,LBL,*14390)       RETURN K 
         .                              END 
         . 
         . 
    66   X(8)=Y(C,K)+CHEBY(Z,*53)       FUNCTION CHEBY(ARG,ERR) 
         .                              . 
         .                              . 
         .                              . 
         ASSIGN 7710 TO RET             RETURN 
         .                              . 
         .                              . 
         .                              . 
    27   G=CHEBY(H,RET)                 RETURN ERR 
         . 
         .                              END 
         . 
When subroutine IT is called by the statement labeled 33, return is to the 
statement with the label 11 if the RETURN J exit is executed or to the 
statement with the label 883 if the RETURN K exit is executed.  When IT is 
called by the statement labeled 48, return is to 55 for the J exit or to 14390 
for the K exit.  When the function subprogram CHEBY is called by the statement 
labeled 66, the first return from the function, if executed, is to the point 
of reference within the statement with the label 66; or the RETURN ERR exit 
transfers to the statement with the label 53. 
00002
00003
00001
Syntax: 
RETURN [e] 
Parameters: 
e    is an integer expression 
Description: 
This form of the RETURN statement is used in a subprogram.  Execution of a 
RETURN statement terminates the reference of a FUNCTION or SUBROUTINE 
subprogram.  Such subprograms may contain more than one RETURN statement; 
however, a subprogram need not contain a RETURN statement.  Execution of an 
END statement in a FUNCTION or SUBROUTINE subprogram has the same effect as 
executing a RETURN statement in the subprogram. 
Execution of a RETURN statement in a FUNCTION subprogram causes return of 
control to the currently referencing program unit.  The value of the function 
must be defined and is available to the referencing program unit. 
Execution of a RETURN statement in a SUBROUTINE subprogram causes return of 
control to the currently referencing program unit.  Return of control to the 
referencing program unit completes execution of the CALL statement. 
Execution of a RETURN statement terminates the association between the dummy 
arguments of the external procedure in the subprogram and the current actual 
arguments.  (See ANS_RETURN_E) 
00002
00001
Syntax 1: 
REWIND {e|(u)|(u,ERR=s,IOSTAT=ios)} 
Parameters: 
e    is an integer, real, or double precision expression, truncated to 
integer.  It is the unit number. 
u    is an integer, real, or double precision expression or the positional 
form of the unit specifier, UNIT=e. 
s    is a statement label. 
ios    is an integer variable or array ID that will be set to an error code 
(see VALUES_OF_IOS) if an error occurs during the REWIND operation. 
Otherwise, it is set to zero. 
The logical unit must be connected to a sequential file.  Note that storage 
files and units connected to direct access files are not permitted. 
Syntax 2: 
REWIND(ERR=s,UNIT=e) 
If the UNIT= form for u is used, the REWIND statement may also take the form 
shown in Syntax 2 above. 
00001
00002
REWIND
In general, each dummy must agree in type with the argument to which it 
corresponds.  For example, the following situation is in error because the 
data types of the arguments do not agree with the data types of the 
corresponding dummies: 
    COMPLEX C     FUNCTION F(LL,CC) 
    LOGICAL L     LOGICAL LL 
    X=F(C,L)      COMPLEX CC 
    .             . 
    .             . 
    .             . 
Reversing the order of either the arguments in the reference or the dummies in 
the FUNCTION statement eliminates the error in this example. 
There are three exceptions to the rule of type correspondence. 
1.  A statement number passed as an argument has no type and may correspond to 
    a dummy variable of any type other than character.  (See also ASSIGN.) 
2.  A SUBROUTINE name (as opposed to a FUNCTION name) has no type if the name 
    has been used in no context other than as a SUBROUTINE.  Therefore, it may 
    correspond to a subprogram of any type, or to another SUBROUTINE that has 
    no type. 
3.  A literal constant also has no type and may be received by any type of 
    dummy array.  Normally, character variables are used for ease of 
    manipulation. 
The following figure is a sample FORTRAN-77 program entered at the user's 
terminal. 
 !FORTRAN 
 FORTRAN 77 VERSION E02  AUG 14 '92 
      10>C ROUTINE TO CALCULATE FACTORIALS 
      20>C 
      30>      INTEGER FACTOR,K 
      40>      K=1 
      50>      READ(1,5)FACTOR 
      60>  10  IF (FACTOR) 12,13,11 
      70>  11  K=K*FACTOR 
      80>      FACTOR=: 
              <>           FACTOR-1 
     100>      GO TO 10 
     110>  12         K=0 
     120>  13  WRITE (108,6) K 
     130>      STOP 
     140>   5  FORMAT (I6) 
     150>   6  FORMAT (' K=',I20) 
     160>      END 
  ERRORS FOUND      : 0      TOTAL ERRORS FOUND: 0 
  Line    Meaning 
  ----    ------- 
  10,20   The character 'C' in column 1 defines these lines as comments. 
  30      A nonexecutable statement that defines to the compiler the 
          variables FACTOR and K as integers. 
  40      An assignment statement that sets K equal to 1. 
  50      An input command that causes the value of FACTOR to be read into 
          storage.  The value is read from unit 1.  The form in which the 
          value of FACTOR appears external to the computer is specified by 
          the FORMAT statement with the label 5 (line 140). 
  60      If FACTOR<0, control is transferred to the statement with the 
          label 12. 
          If FACTOR=0, control is transferred to the statement with the 
          label 13. 
          If FACTOR>0, control is transferred to the statement with the 
          label 11. 
  70      This statement has the label 11 and assigns to K the result of 
          multiplying the value K by the value of FACTOR. 
  80      This line is the initial line of an assignment statement.  The 
          colon character tells the compiler that this statement is to be 
          continued on the next line. 
  (90)    The line number for this statement does not appear as it is a 
          continuation of the prior line and the statement number is the 
          same number as the number of the initial line.  The system 
          spaces over to where column 6 would normally be and outputs the 
          characters '<>'.  The statement continuation is then entered. 
          The ASSIGNMENT statement so completed causes the value of FACTOR 
          to be reduced by 1. 
  100     The line number associated with the continuation line has been 
          skipped.  This statement causes an unconditional transfer of 
          control to the statement with the label 10. 
  110     This statement has the label 12 and assigns the value zero to 
          the variable K. 
  120     This statement has the label 13 and causes the name of the 
          variable K and the value of the variable K to be written out on 
          unit 108.  See line 150 for the FORMAT statement with the 
          label 6. 
  130     This control statement, STOP, will cause the termination of 
          program execution. 
  140     FORMAT statement labeled 5 used in the READ (input) statement on 
          line 50. 
  150     FORMAT statement with the label 6 referenced in the WRITE 
          statement on line 120. 
  160     This END statement denotes the last line in the above program 
          unit; in this case, a main program. 
Syntax: 
SAVE v1,v2,...vn 
Parameters: 
n>0 
vi    is one of the following: 
    1.  A COMMON block name preceded and followed by a slash. 
    2.  An array name. 
    3.  A variable name. 
    4.  A GLOBAL variable or array name. 
If no vi are listed, the SAVE statement is treated as though it contained the 
names of all allowable items in that program unit.  This statement provides 
the programmer with control over variable definition and undefinition in the 
sense of the ANSI standard. 
Description: 
The SAVE statement allows the user to cause local variables in subprograms, as 
well as data COMMON blocks to be loaded into the root data storage areas, and 
guarantees that local data defined in a subprogram will be defined on the next 
entrance to the subprogram. 
The SAVE statement is used to preserve the value of a local variable when a 
RETURN statement is executed.  The next time the subprogram is entered, the 
values of local variables that have been saved are the same as the last time 
that RETURN was executed in the subprogram.  The SAVE statement assures value 
preservation in an overlay-structured program and in a program that allocates 
local variables on entry and deallocates them on return. 
00002
00001
PRINT_POSITIONING
Syntax: 
v=e 
Parameters: 
v    is a variable (or an array element of any type).  v may be a character 
substring name when e is a character expression. 
e    is an expression of an appropriate type 
Description: 
This statement means, "assign to v the value of the expression e."  It is not 
an equation in the true sense; it does not declare that v is equal to e; it 
sets v equal to e.  Thus, the statement 
    N=N+1 
is not a contradiction; it increments the current value of N by 1. 
The expression need not be the same data type as the variable, though in 
practice it usually is.  When it is not, the expression is evaluated 
independently of the data type of the variable.  Then, if permissible, the 
expression value is converted to the data type of the variable according to 
the table shown under MIXED_ASSIGNMENT_TYPES and assigned to the variable. 
00002
00001
SIMPLE_ASSIGNMENT
SIMPLIFIED_IO
Simplified input/output statements include the OUTPUT statement, the INPUT 
statement with list (form 1), the INPUT statement without list (form 2), and 
the NAMELIST READ/WRITE statements.  These statements provide the most 
straightforward form of input/output.  They do not require the programmer to 
learn anything about FORMAT statements or other kinds of input/output which, 
although more versatile, are also more complicated. 
Simplified input/output can be used to process every type of variable and is 
suitable for almost any FORTRAN application that does not require special 
editing or formatting.  Values may be written out in a natural form chosen by 
the compiler.  Similarly, data can be input in a very free form without the 
usual FORTRAN requirement that the user know exactly what format is 
controlling the input operation. 
See OUTPUT 
    INPUT_FORM1 
    NAMELIST_READ 
    NAMELIST_WRITE 
Syntax: 
CALL SLITE(number) 
Parameters: 
number    is an integer expression whose value is between 0 and 35 specifying 
which sense light is to be turned on. 
Description: 
The indicated sense light is turned on.  If number=0, all of the sense lights 
are turned off. 
00002
00001
Syntax: 
CALL SLITET(number,status) 
Parameters: 
number    is an integer expression whose value is between 0 and 35 specifying 
which sense light is to be tested. 
status    is an integer variable into which the value 1 is stored if the sense 
light is on, or the value 2 if it is off. 
Description: 
If SLITET is declared LOGICAL, it may be referenced as a function, returning 
the value .TRUE. if the light is on, .FALSE. if it is off.  Sense light is 
turned off after test. 
00002
00001
CP-6 FORTRAN-77 provides two listing format control declarators:  %PAGE and 
%SPACE.  These allow the user to insert page ejects and blank lines into 
FORTRAN source listings (LS).  The listing control statements operate as 
follows: 
o   %PAGE causes a skip to the top of the next page for the current LS FORTRAN 
    listing. 
o   %SPACE [n] inserts 'n' blank lines into the LS listing where the %SPACE 
    statement is encountered.  The default is '1'. 
Both %PAGE and %SPACE must start in column 1 of the source input report. 
IO_SPECIFIERS
Syntax: 
CALL SSWITCH(number, status)     or CALL SSWTCH(number, status) 
Parameters: 
number    is an integer expression whose value is between 0 and 35 specifying 
which sense switch is to be tested. 
status    is an integer variable into which the value 1 is stored if the sense 
switch is on, or the value 2 if it is off. 
Description: 
If SSWITCH or SSWTCH is declared LOGICAL, it may be referenced as a function, 
returning the value .TRUE. if the switch is on, .FALSE. if it is off.  In this 
case, the second argument is optional. 
00002
00001
SSWITCH
The FORTRAN-77 standard unit assignments are shown below.  There are no 
standard unit assignments for scratch magnetic tapes or random access devices. 
FORTRAN II 
Statement      FORTRAN-77        Standard 
Acceptable     Equivalent        Batch           US Option 
in FORTRAN 77  (Default)         Assignment      Equivalent* 
-------------  ----------        ----------      ----------- 
READ f,k       READ (105,f)k     Card Reader     READ (5,f)k 
PUNCH f,k      WRITE (106,f)k    Card Punch      WRITE (7,f)k 
PRINT f,k      WRITE (108,f)k    Line Printer    WRITE (6,f)k 
READ x         READ (105,x)      Card Reader     READ (5,x) 
ACCEPT x       READ (105,x)      Card Reader     READ (5,x) 
PUNCH x        WRITE (106,x)     Card Punch      WRITE (7,x) 
PRINT x        WRITE (108,x)     Line Printer    WRITE (6,x) 
TYPE x         WRITE (108,x)     Line Printer    WRITE (6,x) 
where 
f    is a format identifier 
k    is an input/output list 
x    is a NAMELIST name 
----------------- 
(*See US compiler option, COMPILER_OPTIONS US. 
For default assignments, enter ? or see DEFAULT_UNIT_SPECIFIERS.) 
DEFAULT_UNIT_SPECIFIER 
The use of a default specifier for a READ/WRITE statement is specified by the 
use of an asterisk (*) in the unit specifier field of the READ/WRITE 
statement. 
This will cause the I/O operation to automatically default to the default 
input/output unit number for the specific processor.  For FORTRAN-77 the 
defaults are: 
     105 for input 
     108 for output 
or if the US option is selected: 
     5 for input 
     6 for output 
Some forms of the use of the default unit specifier are: 
     READ (UNIT=*,FMT=10,END=21)A,B 
     READ(*,FMT=11)I 
     WRITE(UNIT=*,FMT=12)A,B 
     WRITE(*,FMT=33,ERR=21) 
00001
Syntax: 
f(d1,d2,d3,...dn)=e 
Parameters: 
f    is the name of the function. 
di    is the identifier of a dummy variable (see below). 
e    is an expression of any mode that can legally be assigned to data of the 
type of f. 
Description: 
Statement functions are functions that can be defined in a single expression. 
A statement function must have at least one dummy argument.  Statement 
function dummies are treated only as variables; they cannot be dummy arrays or 
dummy subprograms (see ARGUMENTS_DUMMIES).  The expression e should contain at 
least one reference to each dummy.  Other references in the expression are 
unrestricted except that the identifier f may not appear (this would be a 
recursive definition).  In particular, subscripted variables may appear, as 
may references to other statement functions.  Note that those statement 
functions must be defined via a prior appearance. 
Since each di is merely a dummy and does not actually exist, the name of each 
statement function dummy may be the same as the name of any other variable in 
the program, except for the other dummies in the same statement function. 
Note, however, that if a statement function dummy is named X, and there is 
another variable in the program called X, then the appearance of X within the 
statement function expression refers to the dummy.  The only relation between 
a statement function dummy and any other variable with the same name is that 
they will both have the same type.  This enables the programmer to declare the 
type of a statement function dummy by using explicit (or implicit) type 
statements. 
The statement function itself is typed like any other identifier.  It may 
appear in an explicit type statement; if it does not, it will acquire implicit 
type.  (See IMPLICIT_DECLARATIONS.) 
A statement function may be referenced only within the program unit in which 
it is defined.  Statement function definitions must precede all executable 
statements in the program in which they appear. 
Examples: 
    F(X)=A*X**2+B*X+C 
    EI(THETA)=CMPLX(COS(THETA),SIN(THETA)) 
    QTH(OM)=NAME(OM)+ADDR(OM) 
    SWITCH CK(A,B,C)=FLAG(A) .AND. FLAG(B) .AND. FLAG(C) 
00002
00003
00001
A statement label argument is written as: 
kS or &k or *k 
where 
k    is the actual statement label.  The letter S or the preceding ampersand 
or the preceding asterisk distinguishes the construct as a statement label (as 
opposed to an integer constant). 
Within a subprogram, a dummy may be used in much the same way as any other 
variable, array, or subprogram identifier with certain restrictions; namely, 
dummies may not appear in the following types of statements. 
    COMMON       EQUIVALENCE 
    GLOBAL       DATA 
    SAVE         INTRINSIC 
The reason for the above restriction is that dummies do not actually exist. 
Furthermore, classification of a dummy as a variable, an array, or a 
subprogram occurs in the same manner as with other (actual) identifiers, in 
both implicit and explicit classifications.  (See CLASS_OF_IDENTIFIERS.) 
Syntax: 
ASSIGN s TO i 
Parameters: 
s    is a statement label. 
i    is an integer variable name. 
Description: 
Execution of an ASSIGN statement causes the statement label(s) to be assigned 
to the integer variable (i).  The statement label must be the label of a 
statement that appears in the same program unit as the ASSIGN statement.  The 
statement label must be the label of an executable statement or a FORMAT 
statement. 
Execution of a statement label assignment statement is the only way that a 
variable may be defined with a statement label value. 
A variable must be defined with a statement label value when referenced in an 
assigned GO TO statement or as a format identifier in an input/output 
statement.  While defined with a statement label value, the variable must not 
be referenced in any other way. 
An integer variable defined with a statement label value may be redefined with 
the same or a different statement label value or an integer value.  Only 
full-word integer variables may appear in an ASSIGN statement; INTEGER*2 
variables are not allowed. 
Examples: 
    IMPLICIT INTEGER(A-Z) 
    ASSIGN 5 TO JUMP 
    ASSIGN 221 TO M 
    ASSIGN 1234 TO RETURN(J,K) 
    ASSIGN 99999 TO ERROR RECOVERY 
A variable that has most recently had a label assigned to it should not be 
used as a numeric quantity.  Conversely, a variable that has not been assigned 
a label must not be used in any context requiring a label.  For example, the 
following case illustrates improper usage: 
    ASSIGN 703 TO HI 
    A=HI/LOW 
This usage is not permissible because the value of HI is indeterminate; its 
value depends on where the program is loaded.  Furthermore, 
    M=5 
cannot be substituted for 
    ASSIGN 5 TO M 
or vice versa, because the integer constant "5" is implied in the first case, 
and the label "5" in the second. 
00002
00003
00001
Syntax: 
STOP [n|'s'] 
Parameters: 
n    is an unsigned decimal integer and 
's'    is a character constant of up to 72 characters. 
Description: 
This statement terminates the execution of a running program.  If it appears 
within a subprogram, control is not returned to the calling program.  If an 
integer or a message is appended to the STOP statement, it will be output 
immediately before termination.  If nothing follows a STOP statement, the 
following message is printed:  *STOP*. 
00002
00001
STOP
No storage allocation declaration is permitted to cause conflicts in the 
arrangement of storage.  Each COMMON, GLOBAL, and EQUIVALENCE statement 
determines the allocation of the variables referenced in them.  Therefore, no 
EQUIVALENCE set should contain references to more than one variable which has 
previously been allocated.  COMMON and GLOBAL statements should not contain 
references to any variable that has previously been declared to be either in 
COMMON or to be GLOBAL.  When the above rule is not followed, such references 
are either redundant or contradictory and are not allowed. 
For additional information on this topics, enter: 
HELP (FORTRAN) STORAGE_ALLOCATION_RULES subtopic 
where subtopic can be any of the following:  COMMON_EQUIVALENCE, 
GLOBAL_EQUIVALENCE, or DOUBLEWORD_VARIABLE_RULES. 
In all cases, the storage allocation sequence specified in a COMMON statement 
takes precedence over any EQUIVALENCE specifications.  Consequently, 
EQUIVALENCE statements are not allowed to define conflicting allocations of 
COMMON storage. 
It is permissible for an EQUIVALENCE to cause a segment of the COMMON region 
to be lengthened beyond the upper bound established by the last element 
defined to be in that segment.  However, it is not permissible for an 
EQUIVALENCE declaration to cause a segment to be lengthened beneath the lower 
bound established by the first element declared to be in that segment.  Both 
conditions are demonstrated in the following examples. 
    COMMON /BLK1/A(5),B/BLK2/E(4),H,Y(0:3) 
    DIMENSION Z(10),V(5) 
    EQUIVALENCE (A,Z),(V(4),E(2)) 
The first EQUIVALENCE set is a permissible extension of the block BLK1, 
whereas the second set illegally defines an extension of the block BLK2.  The 
declared storage allocation would appear as shown below. 
    Item        BLK1          BLK2 (illegal extension) 
    ----        ----          ------------------------ 
      -                                    V(1) 
      -                                    V(2) 
      1     A(1) = Z(1)             E(1) = V(3) 
      2     A(2) = Z(2)             E(2) = V(4) 
      3     A(3) = Z(3)             E(3) = V(5) 
      4     A(4) = Z(4)             E(4) 
      5     A(5) = Z(5)             H 
      6     B = Z(6)                Y(0) 
      7         Z(7)                Y(1) 
      8         Z(8)                Y(2) 
      9         Z(9)                Y(3) 
     10         Z(10) 
It is assumed that all items are of the same data type. 
The fact that COMMON segments may be lengthened by EQUIVALENCE declarations in 
no way nullifies the requirement that labeled COMMON blocks of the same name 
defined in separate programs or subprograms comprising portions of an 
executable program contain the identical number of words. 
Since each variable declared in a GLOBAL statement is treated as a separate 
block, no two GLOBAL variables may be members of the same EQUIVALENCE set. 
Furthermore, an EQUIVALENCE declaration may not cause the lengthening of a 
GLOBAL block. 
Variables that are members of the same EQUIVALENCE set as a GLOBAL variable do 
not themselves become GLOBAL variables.  In other words, only identifiers 
declared to be GLOBAL become GLOBAL and may be accessed (by name) as such in 
other programs. 
00001
00002
These statements are used to arrange variable and array storage in special 
ways, as required by the programmer.  If no storage allocation information is 
provided, the compiler allocates all variables and arrays within the program 
in an arbitrary order.  The storage allocation statements are 
  COMMON_STATEMENT 
  GLOBAL 
  EQUIVALENCE 
  SAVE 
Dummy variables may not appear in any storage allocation statement. 
Executable subprograms are program units that may be called by other programs. 
They fall into the two broad classes of function subprograms and subroutine 
subprograms, which may be further classified as follows: 
o   Function Subprogram 
    1.  FUNCTION subprograms 
    2.  Intrinsic functions 
    3.  Assembly language and other language (i.e., COBOL) functions 
o   Subprogram Subprograms 
    1.  SUBROUTINE subprograms 
    2.  Assembly language subroutines, and other language subroutines 
o   Functions (other than program units) 
    Statement functions 
Functions are referenced within expressions, and return a value.  See the 
following: 
INTRINSICS 
GENERICS 
SUBROUTINE_STATEMENT 
ENTRY 
FUNCTION_TYPES STANDARD_FUNCTIONS 
FUNCTION_TYPES INTRINSIC_FUNCTIONS 
FUNCTION_TYPES GENERIC_FUNCTIONS 
EXIT 
SSWITCH or SSWTCH 
SLITET 
SLITE 
OVERFL 
DVCHK 
IF_SUBPROGRAM 
DUMP 
PDUMP 
ABORTSET 
EOFSET 
ERROROUT 
PROMPT 
Syntax: 
SUBROUTINE p[(d1,d2,d2,...,dn)] 
Parameters: 
p    is the identifier of the subroutine. 
di    is a dummy argument of any of the forms described under 
ARGUMENTS_DUMMIES. 
Description: 
A SUBROUTINE subprogram may return values to the calling program by assigning 
values to any of the di or the the variables or arrays in COMMON or GLOBAL 
storage. 
It is not necessary for a SUBROUTINE subprogram to contain a RETURN statement. 
Execution of the END statement has the same function as a RETURN. 
Examples: 
SUBROUTINE statement examples: 
    SUBROUTINE CHECK 
    SUBROUTINE VIII(ROMAN) 
    SUBROUTINE AMORTIZE(P,R,N) 
    SUBROUTINE OUTPUT ARRAYS(A,FMT,I,J) 
SUBROUTINE subprogram example: 
    SUBROUTINE QUAD(A,B,C,X1,X2) 
    RAD=SQRT(B**2-4*A*C) 
    X1=(-B+RAD)/(2*A) 
    X2=(-B-RAD)/(2*A) 
    END 
00002
00003
00001
SUBROUTINE subprograms, like FUNCTION subprograms, are self-contained 
programmed procedures.  Unlike FUNCTIONS, however, SUBROUTINE subprograms do 
not have values associated with them, and may not be referenced in an 
expression.  Instead, SUBROUTINE subprograms are accessed by CALL statements. 
A subscript may be any expression of integer, real, double precision, complex 
or double precision complex mode.  Non-integer values used as subscripts are 
truncated to integer values. 
Examples: 
    Array       Subscripts        Array Elements 
    -----       ----------        -------------- 
    MATRIX      (3,9)             MATRIX (3,9) 
    CUBE        (J*4,P,3)         CUBE (J*4,P,3) 
    DATA        (Q/I+U-M)         DATA (Q/I+U-M) 
    PARAM       (-N,-8.0,L,A)     PARAM (-N,-8.0,L,A) 
    J35Z        (75,200)          J35Z (75,200) 
Subscripts may themselves contain array element references to any level. 
    ALPHA (I(J))       BETA (M,P(M),H(B)) 
    I(I(I(I(K))))      SIGMA7(130,P(SIGMA7(1,LL))) 
    X(I(M),J(M),K(M))  S7(L*M(A11(S7)+T(K(L)))) 
00001
CHARACTER_SUBSTRING
A symbolic name is a string of upper- or lower-case letters and decimal 
digits, the first of which must be a letter.  The special characters '_' 
(underscore) and '$' (dollar sign) may also be used in a symbolic name. 
However, the underscore and dollar sign must not be the first character of the 
symbolic name.  Symbolic names are used to name variables, constants, and 
arrays as well as subprograms and COMMON blocks.  In symbolic names, alpha 
characters are case insensitive; that is, lower-case 'a' is the same as 
upper-case 'A'. 
In FORTRAN-77, symbolic names may be any length, but only the first thirty-one 
(31) characters are actually used by the processor.  Therefore, care must be 
taken to assure that no two symbolic names have the same first thirty-one (31) 
characters. 
Examples: 
    X     A345Q     STRESS     J3     MELVIN     ENTROPY     QUANTITY 
    ELEVATION     I     L987654    DIFFERENTIAL     HOUSEKEEP 
Blank characters embedded in symbolic names are ignored:  ONE TIME and ONETIME 
are identical.  (See also IDENTIFIERS.) 
00001
Syntax 
CALL TRUNCATE(value) 
Parameters 
value A value of zero indicates that truncated values are to be put into 
output fields if numeric overflow of the fields occurs.  A value of non-zero 
indicates that overflowed output fields are to be filled with the '*' fill 
character. 
Description 
This subroutine allows users to specify exactly how they wish numeric overflow 
of output fields to be handled. 
00002
00001
GO_TO_UNCONDITIONAL
  These statements process information in internal (binary) form and are 
  designed to provide temporary storage on magnetic tapes and disks.  There 
  are three kinds of unformatted input/output statements:  sequential, direct 
  access, and keyed access.  The simplest forms are 
    Sequential     Direct Access        Keyed Access 
    ----------     -------------        ------------ 
    READ(u)[k]     READ(u,REC=n)[k]     READ(u,KEY=z)[k] 
    WRITE(u)[k]    WRITE(u,REC=n)[k]    WRITE(u,KEY=z)[k] 
Parameters: 
u    is a logical unit number.  It may be any integer, real, or double 
precision expression, truncated to integer if necessary. 
n    is an integer, real, or double precision expression. 
k    is an input/output list, which may be omitted. 
z    is a character expression, or an integer, real or double precision 
expression.  If a character expression, trailing blanks are truncated. 
A comma preceding the list k is optional. 
Description: 
The unformatted READ/WRITE statements process data as a string of internal 
values arranged into words depending on the size of the items in the list k 
(see Allocation of Variables).  All items appearing in the list of a binary 
READ/WRITE statement are contained in one logical record. 
The information output by a single unformatted WRITE statement must be input 
by one and only one unformatted READ statement.  It is permissible to read 
less information that is present in the record, but an error will occur if the 
input list requests more data than is present in a record.  The number of 
items that can be processed by a single sequential unformatted READ/WRITE 
statement is unlimited, but only one logical record will be read or written 
regardless of the amount of data to be transferred.  The number of items that 
can be processed by a single direct-access unformatted READ/WRITE is limited 
by the record size declared for the file. 
The records produced by unformatted WRITE statements consist of just the data 
to be generated.  Control words are not included in the records.  Other 
FORTRAN systems will not necessarily interpret the actual records generated by 
an unformatted WRITE in the same way. 
If the list k is omitted from an unformatted READ/WRITE statement, a record is 
skipped, or an empty record is written.  Unlike formatted input/output 
statements, such an operation can cause no data transfer.  Note that if an 
empty record is written, it can only be processed by a READ statement with no 
list and, therefore, has little purpose. 
Examples: 
Unformatted READ/WRITE statements: 
    Sequential 
    ---------- 
    READ(3)(E1(K),(M(K,L),L=1,22),K=KLOW,KHIGH) 
    READ(N),ARRAY 
    WRITE(3*MIN(J,17))R(J),G(J),24.783,X+SIN(THETA) 
    WRITE(3)VALUE 
    Direct Access 
    ------------- 
    READ(7,REC=K+1)ARRAY 
    READ(M,REC=21),(ARRAY(J),J=K,L) 
    WRITE(10,REC=7)Z,I,E,F 
    WRITE(11,REC=9)12*I,SIN(ALPHA) 
    Keyed Access 
    ------------ 
    READ(7,KEY=K+1)ARRAY 
    READ(M,KEY=21),(ARRAY(J),J=K,L) 
    WRITE(10,KEY=7)Z,I,E,F 
    WRITE(11,KEY=9)12*I,SIN(ALPHA) 
00002
00003
00001
READ_UNFORMATTED
WRITE_UNFORMATTED
STANDARD_UNIT_ASSIGNMENTS
IO_UNIT_ASSIGNMENTS
Value of ios             Condition 
____________             --------- 
Zero            Neither an error condition nor an end-of file 
                condition is encountered by the processor. 
Positive        An error condition is encountered; ios is set 
Integer         to a positive integer value (as returned in 
Value           the Monitor Services ALTRET frame). 
Negative        An end-of-file condition is encountered and 
Integer         no error condition is encountered. 
Value (for 
READ only) 
A variable is a single datum entity and is represented by an identifier whose 
type is the type of the datum. 
Examples: 
    J1     EXPONENT     NAME     SCALAR     EQUATE     E     NEW 
    XXX8 
The following table indicates the amount of storage allocated to each data 
type. 
    Type                Word         Type          Bytes* 
    ----                ----         ----          ----- 
    Integer               1          Logical*1       1 
    Real                  1          Integer*2       2 
    Double precision      2 
    Complex               2 
    Double complex        4 
    Logical               1 
    --------------------- 
    *There are 4 bytes per word. 
Note that character data occupy character storage units, not words.  On the 
Bull (formerly Honeywell) family of large-scale computers, each word contains 
four character storage units. 
Integer *2 declarations specify half-word integers.  Half-word integer scalars 
occupy the two high order bytes of a word.  For half-word integer arrays, two 
array elements fill one word of storage. 
Logical *1 declarations specify byte logicals.  Byte logical scalars occupy 
the high order byte of a word.  For byte logical arrays, four elements occupy 
one word of storage.  Each array element occupies one byte of storage. 
Half-word integers and byte logicals make Bull FORTRAN-77 more compatible with 
other implementations of FORTRAN-77.  However, programs will execute faster 
using full word integers and logicals. 
To view vector intrinsic functions, enter: 
HELP (FORTRAN) VECTOR_INTRINSIC function 
or enter ?? to display them all.  The functions are described in table format, 
with columns for Generic Name, Specific Name, Arguments, Argument Type, Result 
Type (for intrinsic functions) and Definition of Function.  The columns for 
Argument Type and Result Type include the abbreviations which are explained 
below: 
    D     Double precision 
    I     Integer 
    R     Real 
  Gen.   Spec.   Arguments      Arg.    Result   Definition 
  Name   Name                   Type    Type 
 VIPD   IVIPD   (X,Y,N,Ix,Iy)   X,Y:I   I        inner product; 
        RVIPD   (X,Y,N,Ix,Iy)   X,Y:R   R        the sum (Xi*Yi)-> 
        DVIPD   (X,Y,N,Ix,Iy)   X,Y:D   D        proper result register 
 VIPN   IVIPN   (X,Y,N,Ix,Iy)   X,Y:I   I        inner product negated; 
        RVIPN   (X,Y,N,Ix,Iy)   X,Y:R   R        the negated sum (Xi*Yi)-> 
        DVIPN   (X,Y,N,Ix,Iy)   X,Y:D   D        proper result register 
 VSUM   IVSUM   (X,N,Ix)        X:I     I        vector summation; the sum 
        RVSUM   (X,N,Ix)        X:R     R        of the vector X by step 
        DVSUM   (X,N,Ix)        X:D     D        Xi -> proper result 
                                                 register 
 VSN    IVSN    (X,N,Ix)        X:I     I        vector summation negated; 
        RVSN    (X,N,Ix)        X:R     R        the negated sum of the 
        DVSN    (X,N,Ix)        X:D     D        vector X by step Xi -> 
                                                 proper result register 
 VMX    IVMX    (X,N,Ix)        X:I     I        maximum element value 
        RVMX    (X,N,Ix)        X:R     R 
        DVMX    (X,N,Ix)        X:D     D 
 NVMX   NIVMX   (X,N,Ix)        X:I     I        index of element with 
        NRVMX   (X,N,Ix)        X:R     I        maximum value 
        NDVMX   (X,N,Ix)        X:D     I 
 VMN    IVMN    (X,N,Ix)        X:I     I        minimum element value 
        RVMN    (X,N,Ix)        X:R     R 
        DVMN    (X,N,Ix)        X:D     D 
 NVMN   NIVMN   (X,N,Ix)        X:I     I        index of element with 
        NRVMN   (X,N,Ix)        X:R     I        minimum value 
        NDVMN   (X,N,Ix)        X:D     I 
 VMXA   IVMXA   (X,N,Ix)        X:I     I        maximum |element value| 
        RVMXA   (X,N,Ix)        X:R     R        (absolute value) 
        DVMXA   (X,N,Ix)        X:D     D 
 NVMXA  NIVMXA  (X,N,Ix)        X:I     I        index of element with 
        NRVMXA  (X,N,Ix)        X:R     I        maximum absolute value 
        NDVMXA  (X,N,Ix)        X:D     I 
 VMNA   IVMNA   (X,N,Ix)        X:I     I        minimum |element value| 
        RVMNA   (X,N,Ix)        X:R     R        (absolute value) 
        DVMNA   (X,N,Ix)        X:D     D 
 NVMNA  NIVMNA  (X,N,Ix)        X:I     I        index of element with 
        NRVMNA  (X,N,Ix)        X:R     I        minimum absolute value 
        NDVMNA  (X,N,Ix)        X:D     I 
00008
00012
00006
00010
00001
00002
00007
00011
00005
00009
00004
00003
To view vector subroutine information, enter: 
HELP (FORTRAN) VECTOR_SUBROUTINES subroutine 
or enter ?? to display them all.  The subroutines are described in table 
format, with columns for Generic Name, Specific Name, Arguments, Argument 
Type, and Definition of Function.  The columns for Argument Type include the 
abbreviations which are explained below: 
    D     Double precision 
    I     Integer 
    R     Real 
  Gen.   Spec.   Arguments           Input    Output  Definition 
  Name   Name                        Arg.     Arg. 
                                     Type     Type 
 VADD   IVADD   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:I    Z:I     Zi=Xi+Yi 
        RVADD   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:R    Z:R     (vector add) 
        DVADD   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:D    Z:D 
 VSUB   IVSUB   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:I    Z:I     Zi=Xi-Yi 
        RVSUB   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:R    Z:R     (vector subtract) 
        DVSUB   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:D    Z:D 
 VMUL   IVMUL   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:I    Z:I     Zi=Xi*Yi 
        RVMUL   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:R    Z:R     (vector multiply) 
        DVMUL   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:D    Z:D 
 VDIV   IVDIV   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:I    Z:I     Zi=Xi/Yi 
        RVDIV   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:R    Z:R     (vector divide) 
        DVDIV   (Z,X,Y,N,Iz,Ix,Iy)   X,Y:D    Z:D 
 VMA    IVMA    (Z,X,Y,A,N,Iz,Ix,Iy) X,Y,A:I  Z:I     Zi=Xi+Yi*A 
        RVMA    (Z,X,Y,A,N,Iz,Ix,Iy) X,Y,A:R  Z:R     (vector multiply 
        DVMA    (Z,X,Y,A,N,Iz,Ix,Iy) X,Y,A:D  Z:D     and add) 
 VMS    IVMS    (Z,X,Y,A,N,Iz,Ix,Iy) X,Y,A:I  Z:I     Zi=Xi-Yi*A 
        RVMS    (Z,X,Y,A,N,Iz,Ix,Iy) X,Y,A:R  Z:R     (vector multiply 
        DVMS    (Z,X,Y,Z,N,Iz,Ix,Iy) X,Y,A:D  Z:D     and subtract) 
 VITR   IVITR   (Z,X,Y,N,Iz,Ix,Iy)   X,Y,Z:I  Z:I     Z(i+1)=Yi+Xi*Zi 
        RVITR   (Z,X,Y,N,Iz,Ix,Iy)   X,Y,Z:R  Z:R     (vector iterate) 
        DVITR   (Z,X,Y,N,Iz,Ix,Iy)   X,Y,Z:D  Z:D 
 VITN   IVITN   (Z,X,Y,N,Iz,Ix,Iy)   X,Y,Z:I  Z:I     Z(i+1)=Yi-Xi*Zi 
        RVITN   (Z,X,Y,N,Iz,Ix,Iy)   X,Y,Z:R  Z:R     (vector iterate 
        DVITN   (Z,X,Y,N,Iz,Ix,Iy)   X,Y,Z:D  Z:D     and negate) 
 VMV    IVMV    (Z,X,N,Iz,Ix)        X:I      Z:I     Zi=Xi 
        RVMV    (Z,X,N,Iz,Ix)        X:R      Z:R     (vector move) 
        DVMV    (Z,X,N,Iz,Ix)        X:D      Z:D 
        RDVMV   (Z,X,N,Iz,Ix)        X:D      Z:R 
        DRVMV   (Z,X,N,Iz,Ix)        X:R      Z:D 
 VMVC   IVMVC   (Z,X,N,Iz,Ix)        X:I      Z:I     Zi=-Xi 
        RVMVC   (Z,X,N,Iz,Ix)        X:R      Z:R     (vector negate) 
        DVMVC   (Z,X,N,Iz,Ix)        X:D      Z:D 
 VMVA   IVMVA   (Z,X,N,Iz,Ix)        X:I      Z:I     Zi=|Xi| 
        RVMVA   (Z,X,N,Iz,Ix)        X:R      Z:R     (vector absolute 
        DVMVA   (Z,X,N,Iz,Ix)        X:D      Z:D     value) 
 VMVNA  IVMVNA  (Z,X,N,Iz,Ix)        X:I      Z:I     Zi=-|Xi| 
        RVMVNA  (Z,X,N,Iz,Ix)        X:R      Z:R     (vector negative 
        DVMVNA  (Z,X,N,Iz,Ix)        X:D      Z:D     absolute value) 
        VAND    (Z,X,Y,N,Iz,Ix,Iy)   X,Y:I    Z:I     Zi=Xi.AND.Yi 
                                                      (vector "and") 
        VOR     (Z,X,Y,N,Iz,Ix,Iy)   X,Y:I    Z:I     Zi=Xi.OR.Yi 
                                                      (vector "or") 
        VXOR    (Z,X,Y,N,Iz,Ix,Iy)   X,Y:I    Z:I     Zi=Xi.NEQV.Yi 
                                                      (vector 
                                                      "exclusive or") 
00001
00013
00004
00008
00007
00005
00006
00003
00009
00011
00010
00012
00014
00002
00015
Syntax: 
CALL VFCP6(u,flag) 
Parameters: 
u    is an integer value (expression result) that specifies the unit. 
flag    is a logical value that specifies if user VFC control is to turned on 
or off, and must be one of the following: 
    .TRUE. 
    .FALSE. 
Description: 
Calling this subroutine for a given unit with flag set to .TRUE.  allows the 
VFC characters to reach M$WRITE exactly as specified by the user, thereby 
allowing use of all CP-6 Device Format Control characters.  Otherwise, FORTRAN 
turns unrecognized VFC characters into blanks.  The CP-6 VFC capability 
remains active until another call is made with flag set to .FALSE. 
Example: 
    CHARACTER*100 LINEFORT 
    DATA LINEFORT/'@FORTVFC  '/ 
    WRITE(108,'(A100)')LINEFORT 
    CALL VFCP6(108,.TRUE.) 
    WRITE(108,'(A100)')LINEFORT 
    CALL VFCP6 (108,.FALSE.) 
    WRITE(108,'(A100)')LINEFORT 
    END 
00002
00003
00001
VIRTUAL_STATEMENT
Arrays too large to reside completely in memory may be declared virtual 
provided they are other than type character.  The VIRTUAL statement is used to 
declare virtual arrays.  The array identifiers appearing in a VIRTUAL 
statement may also appear in a type statement (other than a CHARACTER 
statement) and a DIMENSION statement.  Virtual arrays may also be used with 
the FORTRAN OPEN, CLOSE, and INQUIRE statements.  See VIRTUAL_ARRAY_STORAGE. 
    There are two kinds of virtual arrays:  those associated with permanent 
    files, and those associated with temporary files.  The association of a 
    virtual array with either a permanent or a temporary (scratch) file 
    becomes important for the following reasons: 
o   The allocation of virtual arrays will be performed in 3 virtual data 
    segments.  Each of these data segments will have 2**32 words available. 
    Therefore, the maximum size of any single virtual array is 2**32 words 
    (2**32 elements for integer, real, or logical virtual arrays; 2**31 
    elements for double precision or complex virtual array; or 2**30 elements 
    for double complex virtual arrays). 
o   Each permanent virtual array has to be assigned to a unique virtual data 
    segment.  Therefore, the maximum number of permanent file virtual arrays 
    that can be active at anyone time is 3 (one array per virtual data 
    segment). 
o   Multiple temporary file virtual arrays may be associated with a given 
    virtual data segment.  The number of temporary file virtual arrays that 
    may be associated with a given data segment is restricted only by total 
    number of words available in a given date segment (that is, 2**32 words of 
    virtual memory). 
The maximum number of virtual arrays that may be active at any one time is 
shown in the following table. 
  Maximum Files Active     Permanent Files     Scratch Files 
       3                    3                    0 
       2+i                  2                    i 
                                            (Combined memory 
                                             f all i scratch 
                                            files must not 
                                            exceed 2**32 words.) 
       1+j                  1                    j 
                                            (All j scratch 
                                            arrays must fit 
                                            into two 2**32 
                                            word virtual array 
                                            segments with no 
                                            overlap between the 
                                            segments.) 
       n                    0                    n 
                                            (All n scratch 
                                            files must fit into 
                                            three 2**32 word 
                                            virtual array 
                                            segments with no 
                                            overlap between 
                                            the segments.) 
Since each variable declared in a VIRTUAL statement is treated as a separate 
block, no two virtual variables may be members of the same EQUIVALENCE set. 
Furthermore, an EQUIVALENCE declaration may not cause the lengthening of a 
virtual block. 
Variables that are members of the same EQUIVALENCE set as a virtual variable 
do not themselves become GLOBAL variables.  In other words, only identifiers 
declared to be VIRTUAL become virtual and may be accessed (by name) as such in 
other programs. 
Virtual arrays may not be equivalenced to data entities that are declared to 
be in COMMON, GLOBAL, or are explicitly SAVEd. 
Virtual arrays may be used with the auxiliary FORTRAN I/O statements OPEN, 
CLOSE, and INQUIRE.  These statements may be used as follows: 
o   The OPEN statement with a virtual array name in the unit number field 
    specifies that a virtual array is to be "OPENed".  That is, a new virtual 
    array is to be allocated at this point, after the old virtual file that is 
    associated with VIRTUAL array is closed (if it existed). 
    The only parameters on the explicit OPEN statement that will be recognized 
    (or make sense), are the following: 
    U (UNIT)     = The virtual array name will be 
                   set in this file. 
    NAME [FILE]  = An explicit file name, other than 
                   the virtual array name, can be 
                   specified here. Only used for 
                   permanent files. 
    STATUS       = Allowed file statuses are SCRATCH to 
                   open a temporary file, and OLD, NEW 
                   and UNKNOWN to open a permanent file. 
                   See Table 9-1 for the interaction 
                   between multiple virtual files. The 
                   default is SCRATCH. 
    ACCOUNT      = Account where the file resides (or 
                   will reside). Only used for permanent 
                   files. 
    USAGE        = Allowed usages include INPUT, 
                   OUTPUT, UPDATE, and CREATE. 
    ERR          = Error return label where control is 
                   to return if an error occurs on the 
                   opening of the file. 
    IOSTAT       = The variable where the error value 
                   will be returned, if an error occurs. 
o   The CLOSE statement that is associated with a VIRTUAL 
    array file, will allow the following I/O specifications. 
    U(UNIT)      = The virtual array name will be set in 
                   this field. 
    STATUS       = Disposition of the file; either 'KEEP' 
                   or 'DELETE'.  The default is 'KEEP'. 
    ERR          = Error return label where control is returned 
                   to the caller if an error occurs. 
    IOSTAT       = Return the error code if an error occurs 
                   on the close of the virtual array. 
o   The INQUIRE statement can be used to inquire about the existence of a 
    specific virtual array.  The INQUIRE by unit (actually virtual array) will 
    be the one recognized by FORTRAN as an inquire about a virtual array's 
    status. 
The parameters recognized by the virtual array INQUIRE statement are as 
follows: 
    U (UNIT=u)     The virtual array name will be set in this 
                   field. 
    EXIST=ex       is a logical variable or logical array 
                   element that becomes defined to TRUE 
                   if the specified virtual array exists 
                   and is known to the run-time processor; 
                   otherwise, ex becomes defined to FALSE. 
    OPENED=op      is a logical variable or logical array 
                   element that becomes defined to TRUE if 
                   a file is connected to the virtual array; 
                   otherwise, op becomes defined to FALSE. 
    ACCOUNT=ac     is a character variable or character 
                   array element that becomes defined to the 
                   name of the account that contains the 
                   file associated with the virtual 
                   array. 
    USAGE=us       is a character variable or character array 
                   element that becomes defined to the 
                   appropriate mode if a file is associated 
                   with the virtual array 
                     'INPUT' for Input mode 
                     'UPDATE' for Update mode 
                     'CREATE' for Create mode 
                   If no file is associated with the virtual 
                   array, the variable becomes undefined. 
    NAME=na        is a character variable or character 
                   array element that becomes defined to 
                   the name of the file if the file has a name; 
                   otherwise, it becomes undefined. 
    ERR=s          is a statement label for the error 
                   specifier.  If an error condition occurs 
                   during execution of an INQUIRE statement 
                   and no IOSTATE=ios specifier is present, 
                   all specified variable elements become 
                   undefined and control is transferred to 
                   the statement with the label s. 
    ERRCODE=ec     Is an integer variable or integer array 
                   element that becomes defined to the error code of 
                   the last operation on this virtual array.  Error 
                   code values are stored as a form of integer constant. 
    IOSTAT=ios     is an integer variable or integer array 
                   element that becomes defined to zero if no 
                   error occurs during execution of the INQUIRE 
                   statement.  If an error occurs during 
                   this execution, the following occurs: 
                      1.  If an IOSTAT=specifier is present, 
                          it is defined to a non-zero value 
                          and normal exit is taken. 
                      2.  If no IOSTAT=specifier is present 
                          and an ERR=specifier is present, 
                          the error exit is taken. 
                      3.  If no IOSTAT=specifier is present and 
                          no ERR=specifier is present, program 
                          execution is terminated. 
Syntax: 
VIRTUAL [(options list)] v1,v2,...vn 
Parameters: 
each vi    is a virtual declarator of one of the following five forms: 
    array identifier 
    array identifier*(options list) 
    array declarator 
    array declarator*(options list) 
    /common block name/ 
The options list may contain any or all of the following options: 
STATUS  = the status and type (temporary or permanent) of a file to be 
          associated with the VIRTUAL array.  See Table 9-1 for the 
          interaction between multiple virtual files. 
ERROR   = the statement label where control is to be returned when an error 
          occurs on the opening of the file associated with the VIRTUAL array. 
BUF     = the number of physical memory pages that are to be associated with 
          the VIRTUAL array.  If not specified, the default size is set at 10. 
IOSTAT  = that the error code is to be returned if an error occurs.  An 
          integer variable name is to be used. 
The values that may be specified with STATUS are as follows: 
    TEMP    implies that the file is a temporary data region. 
    This is the default for VIRTUAL arrays. 
    SCRATCH    is the same as TEMP. 
    OLD    implies that a copy of the data exists from a prior 
    execution. 
    NEW    specifies that a new virtual array file is to be created 
    and saved when the program is executed. 
    UNKNOWN    implies that the status of the file is unknown.  If 
    it exists, it will use an old file.  If it does 
    not exists it will use a new file. 
Description: 
The VIRTUAL statement is used to declare large arrays. 
The array identifiers appearing in a VIRTUAL statement may also appear in the 
following declaration statement: 
o   a type statement, other than a CHARACTER statement 
o   a DIMENSION statement 
The dimension of a virtual array should be declared in the VIRTUAL statement. 
If the array size is larger than 256K words, the dimension declarator must 
appear in the VIRTUAL statement or in a DIMENSION or type statement that 
follows the VIRTUAL statement.  If the array size is less than 256K words, the 
order of appearance of the dimension declarator is not restricted. 
The common block names appearing in a VIRTUAL statement must not appear 
elsewhere in the program unit except as a common block name in the COMMON 
statement.  A common block name that is declared VIRTUAL is restricted from 
identifying any other entity in the program unit in which it is declared.  The 
elements of the common block are not permitted to be of type character. 
Double complex elements in the common block are required to be on a four word 
boundary. 
Specifically, a virtual array name may not appear in 
o   a SUBROUTINE statement 
o   a FUNCTION statement 
o   an ENTRY statement 
o   a GLOBAL statement 
o   a NAMELIST statement 
o   a DATA statement 
o   an EXTERNAL statement 
o   a SAVE statement 
nor may it appear as an element in a COMMON block. 
Usage of a virtual array is restricted in executable statements. 
A virtual array may appear unsubscripted as an argument in an external call, 
or in an I/O list.  However, the maximum amount of words of the array that 
will be passed in the call is 256K.  This is an addressing limitation 
restriction. 
If a common block is declared as a virtual, the common block must be declared 
identically in all subprograms where it is used.  That is, it must contain the 
same variables and arrays, and it must be in the same order. 
A virtual array is a GLOBAL.  If a symbolic name is declared to be a virtual 
array in more than one subprogram in an executable program, each reference to 
that virtual array in any program unit is a reference to the same array.  Note 
that the declared virtual array must have the same size in each program unit 
in which it is declared. 
When a common block is declared to be a virtual array, the shape of the common 
block in all subprograms must be the same.  If the shape of the common block 
is not the same in all subprograms, see HELP (FORTRAN) COMPILER_OPTIONS VCA. 
Example: 
      PROGRAM VT3 
      DIMENSION BB(100000) 
      VIRTUAL BB*(STATUS=NEW,ERROR=240,BUF=200,IOSTAT=IT) 
      DIMENSION ANL(100),ANR(100) 
C     CALCULATIONS FOR VIRTUAL AND NORMAL ARRAYS 
      ANL(100)=30000. 
      ANR(100)=10000. 
      BB(100000)=ANL(100) + ANR(100) 
      OUTPUT BB(100000),ANL(100),ANR(100) 
      CLOSE(UNIT=BB,STATUS='DELETE',ERR=260) 
      STOP 
  240 OUTPUT 'ERROR OCCURS ON VIRTUAL OPEN STATEMENT' 
      STOP 
  260 OUTPUT 'ERROR OCCURS ON CLOSE STATEMENT' 
      STOP 
      END 
00002
00004
00003
00001
00003
Syntax: 
WRITE (specifier_list) [k] 
    or 
WRITE f [,k] 
Parameters: 
specifier_list    is a list of specifiers that includes 
    (1)  A unit specifier 
    (2)  An optional format specifier 
    (3)  An optional error specifier 
    (4)  An optional record specifier 
    (5)  An optional input/output status specifier 
f    is a format identifier 
k    is an input/output list specifying the data to be transferred 
The various forms of the WRITE statement are described under WRITE_FORMATTED, 
WRITE_UNFORMATTED, WRITE_LIST_DIRECTED, WRITE_END_ERR_IOSTAT, WRITE_KEYWORD, 
WRITE_ALTERNATE_DIRECT_ACCESS, and WRITE_NAMELIST. 
Each of these forms have differing requirements for the specifier_list and the 
input/output list. 
00001
Syntax: 
WRITE (u'r[,f])list 
    or 
WRITE (u'r[,f],ERR=k)list 
Parameters: 
u    is a logical unit number.  It may be an integer, real, or double 
precision expression, truncated to integer. 
'    is a field separator required to distinguish the direct-access WRITE from 
other write statements. 
r    is an integer expression for the record number of the file associated 
with the specified logical unit. 
f    is the optional format reference denoting formatted output.  Without f, 
the output is unformatted. 
k    is a statement label. 
list    is an optional I/O list.  The list may not contain the associated 
variable for the logical unit defined in the DEFINE FILE statement. 
Description: 
The direct access WRITE statement is used in conjunction with the DEFINE FILE 
statement.  A FORMAT statement may be referenced in this statement. 
Examples: 
       DEFINE FILE 10(800,100,L,IREC1),20(200,32,L,IREC2) 
       . 
       . 
       . 
       FIND (20'32) 
       . 
       . 
       . 
    20 FORMAT (10I10) 
    11 WRITE (10'20,20)(N(I),I=1,20) 
       . 
       . 
       . 
    13 WRITE (20'IREC2)S,T,U,V,W,X,Y,Z 
The WRITE statement with the label 11 moves data from the array N into the 
file connected to logical unit 10, starting with record 20 of the file, under 
format control specified by the FORMAT statement labeled 20.  Since 200 
characters are to be output and the maximum record size is 100, two records 
are written, 20 and 21.  The associated variable IREC1 is set to a value of 22 
at the conclusion of the operation. 
The WRITE statement with the label 13 moves data from the variables 
S,T,U,V,W,X,Y, and Z into the file associated with logical unit 20, starting 
at record 32 and without format control.  Assuming that all the variables are 
of type real, one record of 32 characters is written.  (If all the variables 
were of type double precision, the specified record size of 32 would have to 
be respecified as 64 to accommodate 64 characters.  If the record size is 32 
and the variables are double precision, two records will be written, record 32 
and record 33.)  The associated variable IREC2 contains a value of 33 at the 
conclusion of the operation. 
As in the READ statement example, the L format specification of the DEFINE 
FILE statement could have been given as E for the file associated with logical 
unit 10, and as U for that associated with logical unit 20.  Similarly, the 
FORMAT statement can be used to control the point where writing is to begin, 
as explained for the READ statement. 
00002
00003
00001
Syntax: 
CALL DIRECT WRITE(u,s,w,k,n,i) 
CALL DIRECT READ(u,s,w,k,n,i) 
Parameters: 
u    is an integer expression that specifies the unit number associated with 
the keyed file that is to be used. 
s    indicates the starting location of the internal buffer.  Normally, s is 
the identifier of an array, but it may also be a variable or an array element. 
w    specifies the number of words to be input or output, starting at s, and 
must be an integer expression whose value is greater than zero. 
k    is the key of the record to be created, replaced, or read.  It may be an 
integer expression whose value lies in the range 0 < k < (2**27)-1. 
n    is optional, but when specified is an integer variable into which is 
stored the number of words actually input or output.  In general, this count 
is significant only for input operations; for output operations, n is normally 
equal to w. 
i    is optional, but when specified is an integer variable into which is 
stored the abnormal or error code, if any, as supplied by the monitor. 
Description: 
A DIRECT WRITE produces a record of exactly the size specified.  If a previous 
record already existed with the same key, it ceases to exist and is entirely 
replaced by the new record.  Thus, it is not possible to write over the first 
part of a record and leave the remainder intact (as is possible with WRITE 
DISK).  An example of a DIRECT WRITE call would be 
CALL DIRECT WRITE(30,MBUF,50,7) 
This creates a record with a key of 7 in the file associated with unit number 
30.  The record is 50 words long and consists of the 50 words beginning at the 
location of MBUF.  As with BUFFEROUT, these words are transmitted exactly as 
they appear in memory.  No formatting is done.  To do BCD formatting from an 
I/O list, an ENCODE statement may be used first to set up the internal buffer. 
Example: 
     INTEGER MBUF(50), ALPHA(25) 
     ENCODE(200,5,MBUF)(ALPHA(K),K=1,20) 
  5  FORMAT(20F10.2) 
Usage Notes: 
Note that the 200 characters specified by the ENCODE statement are equivalent 
to the 50 words specified by the DIRECT WRITE call. 
In general, DIRECT READ should be used only to read records created by a 
DIRECT WRITE.  Attempting to read a key that has not been written results in 
an error. 
Attempting to read more words from a record than it contains results in 
reading the number found; attempting to read fewer words results in reading 
the number requested.  It is preferable to read keyed records using the same 
size with which they were written. 
It is not completely true that the key has no significance as a numeric value; 
there are two places in which it may: 
1.  Under certain operating systems, the DIRECT READ/WRITE subroutines can 
    interface with the standard editing facilities, under which the sequencing 
    of keyed records by value is meaningful. 
2.  Even though the numeric values may not be significant to the system, they 
    may be meaningful to the user's program.  It may be useful to number the 
    keys in some pattern for convenient handling.  For example, suppose that 
    ten records, numbered 1 through 10, have been created in file number 30. 
    Though they may have been created in any order, they can be recalled in 
    numeric order in the following way: 
         INTEGER GRID(50,10) 
         DO 3, K=1,10 
      3  CALL DIRECT READ(30,GRID(1,K),50,K) 
Note that the buffer area must begin at GRID(1,K) and not GRID(K,1), because 
arrays are stored with the first subscript varying most rapidly.  Thus, the 
location following GRID(1,K) is GRID(2,K). 
00002
00001
00003
RANDOM_WORD_ACCESS_STATEMENTS
Syntax: 
     For Sequential WRITE 
     ------------------- 
     WRITE(u,ERR=k2,IOSTAT=ios)list 
     WRITE(u,format,ERR=k2,IOSTAT=ios)list 
     WRITE(u,*,ERR=k2,IOSTAT=ios)list 
     For Direct-Access WRITE 
     ---------------------- 
     WRITE(u,REC=n,ERR=k2,IOSTAT=ios)list 
     WRITE(u,format,REC=n,ERR=k2,IOSTAT=ios)list 
     For Keyed-Access WRITE 
     --------------------- 
     WRITE(u,format,KEY=z,ERR=k2,IOSTAT=ios)list 
     WRITE(u,KEY=z,ERR=k2,IOSTAT=ios)list 
Parameters: 
u    is a logical unit number.  It may be an integer, real, or double 
precision expression, truncated to integer if necessary. 
*    specifies that the operation is list-directed. 
f    is a format reference. 
n    is an integer,real, or double precision expression that has a positive 
value after truncation to integer.  It denotes the record number. 
z    denotes the key value. 
ERR=k2    k2 is a statement label. 
ios    is an integer variable or integer array element. 
Description: 
The formatted, unformatted, and list-directed WRITE statements may optionally 
include the ERR= and IOS= parameters.  ERR=k2 specifies immediate transfer of 
control to statement k2 if an error occurs during the processing of a WRITE 
statement.  Execution of an input/output statement containing the ios 
specifier causes ios to be set as shown below. 
Value of ios             Condition 
------------             --------- 
Zero            Neither an error condition nor an end-of file 
                condition is encountered by the processor. 
Positive        An error condition is encountered; ios is set 
Integer         to a positive integer value (as returned in 
Value           the Monitor Services ALTRET frame). 
Negative        An end-of-file condition is encountered and 
Integer         no error condition is encountered. 
Value (for 
WRITE only) 
00002
00001
Syntax: 
The simplified formatted WRITE statements are written as 
  Sequential        Direct Access          Keyed Access 
  ----------        -------------          ------------- 
  WRITE(u,f)[k]     WRITE(u,f,REC=n)[k]    WRITE(u,f,KEY=z)[k] 
See WRITE_END_ERR_IOSTAT for additional formatted WRITE statement 
forms, and also  WRITE_ALTERNATE_DIRECT_ACCESS. 
Parameters: 
u    is a logical unit number.  It may be any integer, real, or double 
precision expression, truncated to integer if necessary.  In the sequential 
case, it may also be a character variable, a character array name, or a 
character array element; in which case the character entity becomes the 
storage file and the READ statement is interpreted as a storage file I/O 
statement. 
f    is a format reference.  It may be the statement label of a FORMAT 
statement, a variable that has had such a statement label assigned to it, the 
name of an array of a type other than character in which a format has been 
stored (see FORMATS_STORED_IN_ARRAYS under the topic FORMATS), or a character 
variable, a character array, or a character expression. 
k    is an optional input/output list. 
n    is an integer, real, or double precision expression that has a positive 
value after truncation to integer.  It denotes the record number. 
z    denotes the key value and is a character expression or an integer, real, 
or double precision expression, truncated to integer.  If a character 
expression, trailing blanks are truncated and are not included in the key 
value used in the I/O. 
Description: 
A formatted WRITE statement converts internal values (which may be 
expressions) into character strings and outputs them. 
There are three kinds of formatted WRITE statements:  sequential, direct, and 
keyed access. 
Each formatted WRITE statement begins processing with a new record.  Thus, it 
is not possible to process a particular record using more than one WRITE 
statement.  (This can be done with the DECODE and ENCODE statements.)  More 
than one record may be processed by these statements if specifically requested 
by the FORMAT statement.  However, attempting to read more characters on a 
record than are (or can be) physically present does not cause a new record to 
be begun. 
The maximum size of a formatted record depends on the record size for the 
logical unit number.  Certain devices, e.g., card punch and card reader, 
impose absolute restrictions on the size of records:  a punched card contains 
80 characters.  If a record contains no characters, it is considered to be 
blank (or empty); in other words, a record into which any number of blanks 
have been specifically written is indistinguishable within the program from an 
empty record.  Note, however, that on devices such as magnetic tape or paper 
tape, the FORMAT statement may determine the actual size of the record 
written. 
The maximum size of a formatted record for disk files is 133 characters in 
length.  This maximum, which is the default, can be overridden by use of the 
RECL=rl option of the FORTRAN OPEN statement. 
Examples: 
Sequential formatted WRITE statements: 
     WRITE(N,FMT(L))(MASS(J,3),J=100,1,-1),A(4),B 
     WRITE(102,93)MESSAGE,ERROR NUMBER,'AT HOME' 
Direct access formatted WRITE statements: 
     WRITE(19,F,REC=33)(X(I),I=1,21) 
     WRITE(K,A(4),REC=904)(Y(J),J=K,L,2) 
Keyed access formatted WRITE statements: 
     WRITE(19,F,KEY=33)(X(I),I=1,21) 
     WRITE(K,A(4),KEY='SES')(Y(J),J=K,L,2) 
00002
00003
00001
INTERNAL_FILE_WRITE
Syntax: 
WRITE(UNIT=u[,FMT=f][REC=n|KEY=z][,END=k1][,ERR=k2][,IOSTAT=ios])[list] 
Parameters: 
u    is the logical unit number.  May appear in any order. 
f    is the format specifier. 
n    is an integer, real, or double precision expression that has a positive 
value after truncation to integer.  It denotes the record number. 
z    denotes the key value and is a character expression or an integer, real, 
or double precision expression, truncated to integer.  if a character 
expression, trailing blanks are truncated and are not included in the key 
value used in the I/O. 
k1    is a statement label.  If an end-of-file mark is encountered during the 
processing of the WRITE statement, control will be transferred immediately to 
the statement with the label k1. 
k2    is a statement label.  If an error is encountered during the processing 
of the WRITE statement, control will be transferred immediately to the 
statement with the label k2. 
ios    is an integer variable or integer array element.  Execution of a WRITE 
statement containing this specifier causes ios to become to a value as shown 
in VALUES_OF_IOS. 
list    is an input/output list (see IO_LISTS). 
Description: 
Keyword forms are alternate forms of the READ and WRITE statement.  The 
keyword form for unit number is UNIT=u, where u is the logical unit number, 
and FMT=f for the format specifier where f is the format specifier. 
If both the unit number and format specifier appear in a WRITE statement and 
the keyword form UNIT=u is used, the keyword form FMT=f must also be used. 
FMT=f and UNIT=u specifier may appear in any order among the other specifiers 
(e.g., the ERR= and, if present, the END=, REC= or the KEY=). 
Examples: 
     Formatted Sequential WRITEs 
     --------------------------- 
           WRITE(UNIT=10,FMT=L,ERR=21)'POSITION',A,B,X 
           WRITE(UNIT=11,ERR=9,IOSTAT=IJK,FMT=27)A+B,C(I),J*K 
           WRITE(FMT=901,ERR=77,UNIT=11)A*B,C,D/E,F 
     Formatted Direct-Access WRITEs 
     ------------------------------ 
           WRITE(UNIT=11,FMT=27,REC=91,ERR=92)A,B,C 
           WRITE(FMT=21,REC=9,UNIT=11)Z1,Z11,Z2,Z21 
           WRITE(REC=11,FMT=17,ERR=101,UNIT=U)A1,A2,A3 
     Formatted Keyed-Access WRITEs 
     ----------------------------- 
           WRITE(UNIT=11,FMT=71,KEY=J)K,L,M 
           WRITE(KEY=21000,UNIT=A,FMT=B)ALPHA,BETA,Z1 
           WRITE(ERR=101,KEY=21500,UNIT=A,FMT=B)GAMMA,DELTA 
     Unformatted Sequential WRITEs 
     ----------------------------- 
           WRITE(UNIT=3,ERR=12)SIN(X),X,Y 
           WRITE(UNIT=I,ERR=21)Z,Z1,W 
     Unformatted Direct-Access WRITEs 
     -------------------------------- 
           WRITE(UNIT=U1,REC=IRECORD)Z(1),Y(1) 
           WRITE(REC=JRECORD,UNIT=12,ERR=7)X,Y,Z 
     Unformatted Keyed-Access WRITEs 
     ------------------------------- 
           WRITE(UNIT=U1,KEY=C//1'ABC')Z(1),X(1) 
           WRITE(KEY=I,UNIT=3,ERR=11)A,B,C 
     List Directed Sequential WRITEs 
     ------------------------------- 
           WRITE(UNIT=11,FMT=*,ERR=21,IOSTAT=IK)ALP,MONT,BLANC 
           WRITE(FMT=*,UNIT=X,ERR=23)X,Y,Z 
00002
00003
00001
READ_LIST_DIRECTED
NAMELIST_WRITE
WRITE
Syntax: 
The simplest forms are: 
  Sequential     Direct Access        Keyed Access 
  ----------     -------------        ------------ 
  WRITE(u)k]     WRITE(u,RC=n)[k]     WRITE(u,KEY=z)[k] 
See also WRITE_END_ERR_IOSTAT for additional forms of the 
unformatted WRITE statement. 
Parameters: 
u    is a logical unit number.  It may be any integer, real, or double 
precision expression, truncated to integer if necessary. 
n    is an integer, real, or double precision expression. 
k    is an input/output list, which may be omitted. 
z    is a character expression, or an integer, real or double precision 
expression.  If a character expression, trailing blanks are truncated. 
A comma preceding the list k is optional. 
Description: 
The unformatted WRITE statements process information in internal (binary) form 
and are designed to provide temporary storage on magnetic tapes and disks. 
The three kinds of unformatted WRITE statements are sequential, direct access, 
and keyed access. 
The unformatted WRITE statements process data as a string of internal values 
arranged into words depending on the size of the items in the list k (see 
VARIABLE_ALLOCATION).  All items appearing in the list of a binary WRITE 
statement are contained in one logical record. 
The information output by a single unformatted WRITE statement must be input 
by one and only one unformatted READ statement.  It is permissible to read 
less information that is present in the record, but an error will occur if the 
input list requests more data than is present in a record.  The number of 
items that can be processed by a single sequential unformatted READ/WRITE 
statement is unlimited, but only one logical record will be read or written 
regardless of the amount of data to be transferred.  The number of items that 
can be processed by a single direct-access unformatted READ/WRITE is limited 
by the record size declared for the file. 
If the list k is omitted from an unformatted READ/WRITE statement, a record is 
skipped, or an empty record is written.  Unlike formatted input/output 
statements, such an operation can cause no data transfer.  Note that if an 
empty record is written, it can only be processed by a READ statement with no 
list and, therefore, has little purpose. 
Examples: 
Unformatted READ statements: 
     Sequential 
     ---------- 
     READ(3)(E1(K),(M(K,L),L=1,22),K=KLOW,KHIGH) 
     READ(N),ARRAY 
     Direct Access 
     ------------- 
     READ(7,REC=K+1)ARRAY 
     READ(M,REC=21),(ARRAY(J),J=K,L) 
     Keyed Access 
     ------------ 
     READ(7,KEY=K+1)ARRAY 
     READ(M,KEY=21),(ARRAY(J),J=K,L) 
00002
00003
00001
