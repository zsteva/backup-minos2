REM CommandInterpreter: $(COMSPEC)
if exist C:\"Program Files"\Git\bin\git ( 
C:\"Program Files"\Git\bin\git rev-parse HEAD >gitver.in
) else (
  C:\"Program Files (x86)"\Git\bin\git rev-parse HEAD >gitver.in
)
embedGit\embedGitVersion gitver.in gitver.h
del MinosVer.res
brcc32 MinosVer.rc
