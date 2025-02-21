A new feature that allows users define their custom types and use them. Custom types have a lot of features like custom constructors, casters and type inheritance.

## Example of Defining a Custom Type

In order to define custom type, you need to decide what base type you will use for your type. 

### Built-In Types (at the moment)
1. int
2. float
3. double
4. bool
5. string

After you decided which type you want to use, you can define your type using following syntax:

```
typedf Integer = int;
```

In the above example we've defined custom type named `Integer` that inherits from `int`. That means that `int` is now the base type of `Integer`

## Using Your Custom Type

After type definition you should declare type's constructor in order to define variables within that type.

```
fn @construct Integer(int value) : Integer {
    return value;
}
```

Your constructor should return your type's base type. In above example the constructor of `Integer` should return `int`, because `int` was specified as base type that `Integer` inherits from.

After that you can define a variable with your custom type

```
->main {
    var Integer test = new Integer(50);
    # use your type...
}
```

## Type Casting

In order to cast one type to another you need to use following syntax

```
->main {
    var float PI = 3.1415;
    var int PI_int = (int)PI;
    println(PI_int.tostring()); # expected "3"
}
```

Going further with above example, you can do down-casting any time for any custom defined type. Down-casting is a casting of the type to it's parent type (base type).

Here's an example of down-casting

```
typedf Integer = int;

fn @construct Integer(int value) : Integer {
    return value;
}

->main {
    var Integer test = new Integer(50);
    var int native = (int)test;
    println(native.tostring()); # expected "50"
}
```

In above example down-casting to `int` will only work if `Integer` inherits from `int`.

## Custom Type Casters

If you want your type to be casted to another type, for example our `Integer` to `string`, then you can implement custom caster for your type

Here is an example of creating custom type caster for your own type

```
typedf Integer = int;

fn @construct Integer(int value) : Integer {
    return value;
}

fn @cast(Integer) -> string {
    return ((int)self).tostring();
}

->main {
    var Integer test = new Integer(100);
    var string data = (string)test;
    println(data); # expected: "100"
}
```

