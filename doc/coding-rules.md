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
      and an underscore (e.g., sensor_read()).

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
