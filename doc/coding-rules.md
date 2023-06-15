## Coding rules

### 1. Modules:
1.1 Naming conventions:
    - All module names shall consist entirely of lowercase letters,
      numbers, and underscores. No spaces shall appear within the
      module’s header and source file names.
    - All module names shall be unique in their first 8 characters and end with
      suffices .h and .c for the header and source file names, respectively.
    - No module’s header file name shall share the name of a header file
      from the C Standard Library or C++ Standard Library. For example,
      modules shall not be named “stdio ” or “math ”.
1.2 Rules:
    - There shall always be precisely one header file for each source file,
      but many source file may have the same header file.
    - Each header file shall contain a preprocessor include guard.
    - The headerfile shall identify only the functions, constants and data types
      about which it is strictly necessary for other modules to be informed.
    - Each header file should include only necessary modules with required type definitions.
    - Each header file should be free of unused include files.
    - Each source file shall contain some or all of the following sections,
      exactly in the order listed:
      * comment block with module description
      * include statements
      * global variables
      * private function declarations
      * public functions definitions
      * private functions definitions

### 2. Functions:
2.1 Naming conventions:
    - No procedure shall have a name that begins with an underscore.
    - No procedure name shall be longer than 31 characters.
    - No function name shall contain any uppercase letters.
    - No macro name shall contain any lowercase letters.
    - Underscores shall be used to separate words in procedure names.
    - No procedure shall have a name that is a keyword of any standard
      version of the C or C++ programming language. Restricted names
      include interrupt , inline , class, true, false, public, private ,
      friend, protected , and many others.
    - No procedure shall have a name that overlaps a function in the C
      Standard Library. Examples of such names include strlen, atoi,
      and memset.
    - The names of all public functions shall be prefixed with their module name
      and an underscore (e.g., gpio_read()).
2.2 Rules:
    - Functions should be as short as possible, no longer than 100 lines.
    - All private functions should be declared as `static`
    - All public functions should be declared in module header file.
    - Each parameter should have meaningfull name.
    - Each function should be preceded by a comment with short description
      of its behavion, parameters and return value.

### 3. Data types:
3.1 Naming conventions:
    - All new structures, unions, and enumerations shall be named via a typedef.
    - All data types names shall begin with lowercase letters and each next word
      shall begin with uppercase letters (no undersores).
    - Structures, enums and unions shall end with `S`, `E` and `U` respectively.
    - The name of all public data types shall be prefixed with their module name
      and an underscore.

### 4. Variables:
4.1 Naming conventions:
    - No variable shall have a name that begins with an underscore.
    - No variable name shall be longer than 31 characters.
    - No variable name shall contain any uppercase letters.
    - Underscores shall be used to separate words in variable names.
    - The names of any pointer variables shall end with `_p`
    - The names of any global variables shall end with `_g`
4.2 Rules:
    - All variables should be initialized at declaration.
    - Each variable should be defined in separate line.
    - All variables should have minimal required scope.
    - All local variables should be defined at the top of their scope.