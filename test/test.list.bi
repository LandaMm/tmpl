:i count 44
:b shell 12
tmpl nothing
:i returncode 1
:b stdout 0

:b stderr 63
[1;31mProcedureError[0m: No procedure found with name 'main'

:b shell 20
tmpl empty-procedure
:i returncode 0
:b stdout 42
[DEBUG] evaluated procedure: <EmptyValue>

:b stderr 0

:b shell 37
tmpl proc-main && tmpl proc-main main
:i returncode 0
:b stdout 84
[DEBUG] evaluated procedure: <EmptyValue>
[DEBUG] evaluated procedure: <EmptyValue>

:b stderr 0

:b shell 9
tmpl expr
:i returncode 0
:b stdout 46
[DEBUG] evaluated procedure: IntegerValue(-6)

:b stderr 0

:b shell 16
tmpl literal int
:i returncode 0
:b stdout 49
[DEBUG] evaluated procedure: IntegerValue(45876)

:b stderr 0

:b shell 18
tmpl literal float
:i returncode 0
:b stdout 50
[DEBUG] evaluated procedure: FloatValue(3.141500)

:b stderr 0

:b shell 19
tmpl literal double
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: DoubleValue(4.583297)

:b stderr 0

:b shell 19
tmpl literal string
:i returncode 0
:b stdout 58
[DEBUG] evaluated procedure: StringValue("hello, world!")

:b stderr 0

:b shell 18
tmpl literal false
:i returncode 0
:b stdout 46
[DEBUG] evaluated procedure: BoolValue(false)

:b stderr 0

:b shell 17
tmpl literal true
:i returncode 0
:b stdout 45
[DEBUG] evaluated procedure: BoolValue(true)

:b stderr 0

:b shell 12
tmpl var int
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: IntegerValue(5068593)

:b stderr 0

:b shell 14
tmpl var float
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: FloatValue(50.685928)

:b stderr 0

:b shell 15
tmpl var double
:i returncode 0
:b stdout 52
[DEBUG] evaluated procedure: DoubleValue(50.685935)

:b stderr 0

:b shell 15
tmpl var string
:i returncode 0
:b stdout 58
[DEBUG] evaluated procedure: StringValue("hello, world!")

:b stderr 0

:b shell 27
tmpl condition true_compare
:i returncode 0
:b stdout 45
[DEBUG] evaluated procedure: BoolValue(true)

:b stderr 0

:b shell 28
tmpl condition false_compare
:i returncode 0
:b stdout 46
[DEBUG] evaluated procedure: BoolValue(false)

:b stderr 0

:b shell 24
tmpl condition true_less
:i returncode 0
:b stdout 45
[DEBUG] evaluated procedure: BoolValue(true)

:b stderr 0

:b shell 25
tmpl condition false_less
:i returncode 0
:b stdout 46
[DEBUG] evaluated procedure: BoolValue(false)

:b stderr 0

:b shell 27
tmpl condition true_greater
:i returncode 0
:b stdout 45
[DEBUG] evaluated procedure: BoolValue(true)

:b stderr 0

:b shell 28
tmpl condition false_greater
:i returncode 0
:b stdout 46
[DEBUG] evaluated procedure: BoolValue(false)

:b stderr 0

:b shell 26
tmpl condition true_lesseq
:i returncode 0
:b stdout 45
[DEBUG] evaluated procedure: BoolValue(true)

:b stderr 0

:b shell 27
tmpl condition false_lesseq
:i returncode 0
:b stdout 46
[DEBUG] evaluated procedure: BoolValue(false)

:b stderr 0

:b shell 29
tmpl condition true_greatereq
:i returncode 0
:b stdout 45
[DEBUG] evaluated procedure: BoolValue(true)

:b stderr 0

:b shell 30
tmpl condition false_greatereq
:i returncode 0
:b stdout 46
[DEBUG] evaluated procedure: BoolValue(false)

:b stderr 0

:b shell 23
tmpl condition true_not
:i returncode 255
:b stdout 0

:b stderr 114
[90m[.tmpl/condition.tmpl:43:12][0m [1;31mRuntimeError[0m: Unary operator '!' is not allowed with type 'bool'

:b shell 24
tmpl condition false_not
:i returncode 255
:b stdout 0

:b stderr 114
[90m[.tmpl/condition.tmpl:47:12][0m [1;31mRuntimeError[0m: Unary operator '!' is not allowed with type 'bool'

:b shell 17
tmpl ternary left
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: StringValue("works!")

:b stderr 0

:b shell 18
tmpl ternary right
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: StringValue("works!")

:b stderr 0

:b shell 23
tmpl ternary inner_left
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: StringValue("works!")

:b stderr 0

:b shell 24
tmpl ternary inner_right
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: StringValue("works!")

:b stderr 0

:b shell 18
tmpl unary neg_int
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: IntegerValue(-578957)

:b stderr 0

:b shell 20
tmpl unary neg_float
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: FloatValue(-3.141500)

:b stderr 0

:b shell 21
tmpl unary neg_double
:i returncode 0
:b stdout 57
[DEBUG] evaluated procedure: DoubleValue(-656788.448789)

:b stderr 0

:b shell 22
tmpl unary dbl_neg_int
:i returncode 0
:b stdout 45
[DEBUG] evaluated procedure: IntegerValue(1)

:b stderr 0

:b shell 24
tmpl unary dbl_neg_float
:i returncode 0
:b stdout 50
[DEBUG] evaluated procedure: FloatValue(3.141500)

:b stderr 0

:b shell 25
tmpl unary dbl_neg_double
:i returncode 0
:b stdout 54
[DEBUG] evaluated procedure: DoubleValue(3487.141547)

:b stderr 0

:b shell 20
tmpl if-else if_true
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: StringValue("works!")

:b stderr 0

:b shell 21
tmpl if-else if_false
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: StringValue("works!")

:b stderr 0

:b shell 22
tmpl if-else else_case
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: StringValue("works!")

:b stderr 0

:b shell 22
tmpl if-else true_else
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: StringValue("works!")

:b stderr 0

:b shell 23
tmpl if-else false_else
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: StringValue("works!")

:b stderr 0

:b shell 18
tmpl if-else inner
:i returncode 0
:b stdout 51
[DEBUG] evaluated procedure: StringValue("works!")

:b stderr 0

:b shell 11
tmpl return
:i returncode 0
:b stdout 45
[DEBUG] evaluated procedure: IntegerValue(1)

:b stderr 0

:b shell 14
tmpl fn params
:i returncode 0
:b stdout 50
[DEBUG] evaluated procedure: FloatValue(3.141500)

:b stderr 0

