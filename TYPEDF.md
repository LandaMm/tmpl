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
