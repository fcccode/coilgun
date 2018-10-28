# COILGUN

It's not finished at all. 20% done.

## Description
Tool allows you to call Windows API without additional compilations. Moreover it gives you opportunity for advanced usage, i.e variable creation, struct definition.

## Command reference

### add
#### types

To create new type, that's not yet defined in the program you need to use the following command:<br>
`create type TYPENAME TYPESIZE [OUTPUTFORMAT]`<br>
Output format is optional. You can set it to these options:
* hex (default)
* int
* str
Later you will be able to reference this type when creating variables.

#### variables
When creating variable you need to be cautious. You have following options when creating them:
* `add var VARAIBLETYPE VARIABLENAME 0xFEEDFACE`
* `add var VARIABLETYPE VARIABLENAME "Example String"`
* `add var VARIABLETYPE VARIABLENAME *VARIABLENAME`
* `add var VARIABLETYPE VARIABLENAME 1234`
* `add var VARIABLETYPE VARIABLENAME `

So, you can create variables with input data in 
1) Hexadecimal
2) String
3) Pointer to another variable
4) Integer
5) Empty variable (will contain null)

#### structs
Work in progress

#### function definition
Before calling  function you need to define it:
`add func DLLNAME FUNCTION_NAME ARGC RETURN_TYPE`<br>
if you don't care about return type use "NULL"

### delete
Work in progress

### edit
Work in progress

### call
Before calling any function make sure to load corresponding library and resolve the function (look at "load" and "add func" commands)<br>
To call a fuction use this command with function name:
`call GetCurrentProcessId`<br>
If function requires arguments then the tool will ask you.
To pass function argument by reference use "&" right before the variable name.<br>
If you specified type other than "NULL" when adding  a function, then the tool will push the return value into the variable list (please note that the previous return value is getting deleted).


### quickcall
Work in progress


### load
Use this command to load DLLs. Example:
`load ntdll.dll`
`load kernel32.dll`

### shell
You can quickly drop to shell by using this command.

### help
Help without arguments will print available commands. To get more info for specific command use help with command's name

### print
You can print:
* loaded libraries
* defined types
* structures
* allocated variables
* resolved functions
* variables value

### exit
To exit :)

