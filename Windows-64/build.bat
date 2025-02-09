gcc -O3 -c toggle.c -o toggle.o
if %ERRORLEVEL% NEQ 0 (
    exit /b
)
gcc -O3 -o toggle  toggle.o -L ../w64devkit/x86_64-w64-mingw32/lib -L .  -lfreeglut -lglu32 -lopengl32 -lgdi32
