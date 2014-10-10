REM CommandInterpreter: $(COMSPEC)
if exist C:\"Program Files"\Git\bin\git.exe (
C:\"Program Files"\Git\bin\git.exe rev-parse HEAD >gitver.in
goto 1
)
C:\"Program Files (x86)"\Git\bin\git.exe rev-parse HEAD >gitver.in
:1

embedGit\embedGitVersion gitver.in gitver.h
del MinosVer.res
brcc32 MinosVer.rc
