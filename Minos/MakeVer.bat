REM CommandInterpreter: $(COMSPEC)
REM cd ..\adjsql
C:\"Program Files"\Git\bin\git rev-parse HEAD >gitver.in
embedGit\embedGitVersion gitver.in gitver.h
del MinosVer.res
brcc32 MinosVer.rc
