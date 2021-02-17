# Compilador con Análisis Léxico, Sintáctico, Semántico y Generación de Código Intermedio
### _Proyecto Final de Compiladores 2021-1_

**Alumno**: Gutiérrez Velázquez Héctor Ernesto. 

Proyecto realizado en su totalidad por mí, como consecuencia de que del equipo original dos compañeros dieron de baja la materia y el tercero no se comunicó conmigo desde diciembre. 

Para su compilación y ejecución se incluye un _Makefile_ dentro de la carpeta _src_. 
Para compilar la práctica ejecute desde dicha carpeta simplemente el comando
```sh
make
```
Esto generará un ejecutable de nombre _parser__, el cual puede ejecutar haciendo
```sh
./parser <archivo_entrada>
```

Se incluyen archivos de prueba muy sencillos dentro de _src/pruebas_. Son ejemplos muy sencillos
con la intención de que se pueda apreciar que el código intermedio genera de manera correcta las
etiquetas y las instrucciones para las partes más importantes de la gramática. 

- El compilador recibe archivos (idealmente con terminación .in, pero es libre) de texto con el lenguaje especificado por la gramática (presente en los documentos)
- Realiza el análisis léxico y sintáctico, este último por el método recursivo descendiente.
- Como parte del asemántico hace verificación de tipos en las operaciones (tanto de asignación como lógicas y aritméticas) y verifica el correcto acceso a los índices de arreglos. También se verifican los tipos de retorno de las funciones y los parámetros en las llamadas de funciones.
- Genera un archivo con el mismo nombre que la entrada, pero con terminación .ci donde se encuentra un código intermedio (impreso en formato de tres direcciones, pero generado usando cuádruplas).
