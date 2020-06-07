# pcc - simple pascal-like compiler based on LLVM

## How to build

### linux & macOS & FreeBSD

- download the LLVM 10.0.0 prebuilt binary at [LLVM release page](https://releases.llvm.org/download.html#10.0.0)
- extract the tarball
- make a build directory under the source directory
- run `cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/the/llvm/`
- run `cmake --build . -j8`

### windows

- get the awesome package manager [vcpkg](https://github.com/Microsoft/vcpkg)
- run .\vcpkg.exe install llvm:x64-windows
- make a build directory under the source directory
- run `cmake .. -DCMAKE_TOOLCHIAN_FILE=/path/to/your/vcpkg/toolchain/file`
- run `cmake --build . -j8`

### How to run

```
Usage: pcc [-vVhiScad] [-O <0,1,2,3>] [-o <output>] file...
    -v            Verbose mode.
    -vv           Very verbose mode.
    -V            Show version and exit.
    -h            Show this help and exit.

    -i            Generate LLVM IR (default behavior).
    -S            Compile to assembly file.
    -c            Compile to object file.
    -o <output>   Specify output file name.
    -O <0,1,2,3>  Specify optimization level.

    -a            Dump the AST.
    -d            Generate a dot file for AST.
```

You can find some examples at the t directory.

#### compile pascal file to executable

```
./pcc -c qsort.pas
```

Then you need to link the object file qsort.o by your host-compiler.

- gcc case: `gcc -no-pie qsort.o`
- clang case: `clang qsort.o`
- msvc case: find the developer command window, use cl to link

#### calling C functions from pascal

See ssl.pas and extern.pas.

For the `scanf` case, the `scanf` may be a marco in C like `__isoc99_scanf`, so you may need to wrap the scanf function, see common.c.

#### calling pascal functions from C

See sha1.pas and sha1_driver.pas.

### How to get the tree or graph view of AST

pass -a to pcc, you will get a tree view of AST like this:

```
  `-FunctionNode FunctionName=main ReturnType=integer
    |-VarDeclNode local size=2
    | |-Variable name=arr type=integer[100000]
    | `-Variable name=i type=integer
    `-StatementListNode size=5
      |-FunctionCallNode FunctionName=srand ResultType=void
      | `-FunctionCallNode FunctionName=time ResultType=integer
      |   `-IntegerLiteralNode value=0
      |-ForStatementNode direction=up
      | |-IdentifierNode lvalue name=i type=^integer
      | |-IntegerLiteralNode value=0
      | |-IntegerLiteralNode value=99999
      | `-AssignStatementNode ValueType=int64
      |   |-ArrayAccessNode ResultType=^integer
      |   | |-IdentifierNode lvalue name=arr type=^integer[100000]
      |   | `-L2RCastingNode ResultType=integer
      |   |   `-IdentifierNode lvalue name=i type=^integer
      |   `-BinaryExprNode op='%' ResultType=int64
      |     |-FunctionCallNode FunctionName=rand ResultType=integer
      |     `-IntegerLiteralNode value=1000
      |-FunctionCallNode FunctionName=qsort ResultType=void
      | |-IdentifierNode lvalue name=arr type=^integer[100000]
      | |-IntegerLiteralNode value=0
      | `-IntegerLiteralNode value=99999
      |-ForStatementNode direction=down
      | |-IdentifierNode lvalue name=i type=^integer
      | |-IntegerLiteralNode value=99999
      | |-IntegerLiteralNode value=0
      | `-FunctionCallNode FunctionName=printf ResultType=integer
      |   |-StringLiteralNode value='%d \x00'
      |   `-L2RCastingNode ResultType=integer
      |     `-ArrayAccessNode ResultType=^integer
      |       |-IdentifierNode lvalue name=arr type=^integer[100000]
      |       `-L2RCastingNode ResultType=integer
      |         `-IdentifierNode lvalue name=i type=^integer
      `-AssignStatementNode ValueType=int64
        |-IdentifierNode lvalue name=main type=^integer
        `-IntegerLiteralNode value=0
```

pass -d to pcc, it will generate a dot file, use graphviz to render it.

```
pcc -d qsort.pas
dot -Tpng -o qsort.png qsort.dot
```
