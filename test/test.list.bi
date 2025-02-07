:i count 14
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
:i returncode 255
:b stdout 0

:b stderr 99
[90m[.tmpl/var.tmpl:8:7][0m [1;31mTypeError[0m: Cannot redeclare already existing variable 't'

:b shell 14
tmpl var float
:i returncode 255
:b stdout 0

:b stderr 99
[90m[.tmpl/var.tmpl:8:7][0m [1;31mTypeError[0m: Cannot redeclare already existing variable 't'

:b shell 15
tmpl var double
:i returncode 255
:b stdout 0

:b stderr 99
[90m[.tmpl/var.tmpl:8:7][0m [1;31mTypeError[0m: Cannot redeclare already existing variable 't'

:b shell 15
tmpl var string
:i returncode 255
:b stdout 0

:b stderr 99
[90m[.tmpl/var.tmpl:8:7][0m [1;31mTypeError[0m: Cannot redeclare already existing variable 't'

