Development environment:
- Ubuntu 20 running from Windows WSL (version 1)
- VSCode with remote plugin to WSL (configuration files for VSCode are in .VSCode folder in Src)
- g++ from GCC with some pure C code
- cross compiling in MSYS2 MinGW64 

Building, Compiling
- just cd to Src dir and run make 
- Linking: -lgmp -lstdc++fs -lpthread -lcurl -ljsoncpp
- In case custom GMP is needed: -L/gmp_install/lib 

Linux packages for development
- sudo apt install libjsoncpp-dev
- sudo apt install libcurl4-openssl-dev
- sudo apt install libgmp3-dev
- sudo apt-get install libncurses5-dev libncursesw5-dev
- and other basic packages: git, latest gcc and g++, make, gdb,...

Fresh install of MSYS2 (64 bit)
- https://www.msys2.org/
- https://stackoverflow.com/questions/45836650/how-do-i-integrate-msys2-shell-into-visual-studio-code-on-window
- pacman -Syu
- pacman -Su
- pacman -S --needed base-devel mingw-w64-x86_64-toolchain
- pacman -S jsoncpp-devel

Certificates 
- pacman -S ca-certificates
- pacman -S mingw-w64-x86_64-ca-certificates
- https://curl.se/docs/sslcerts.html
- still not working under Windows so cacert is copied to bin folder and it works
- newer version of libcurl have a better windows cert support


Adding execute right under Linux:
- chmod a+rx my-script.sh

Counting number of lines:
- cat */*.c */*.cpp */*.hpp */*.sql */*php */*.txt | wc -l

Links:
- Readme in GoogleDocs https://docs.google.com/document/d/1zrb4HBXrz3D-NUTpIV1X1VHL79Y44St39ds0p36apJ4/edit


**Git Help**

1. git init
2. git config --global user.email "your_email_address@example.com"
3. udelat repo
4. git remote add origin https://20gZeman@bitbucket.org/20gZeman/test.git


Standard workflow
git checkout -b "taskX"
git add .
git commit -m "taskX done"
git checkout master
git merge taskX
smazání lokální větve
git branch -d taskX
git push

git check-ignore
git check-ignore -v example.o Readme.md
Plus the rule why the file is being ingored

git status --ignored
git status --ignored --untracked-files=all
including all subdirs

deletion of the remote branch taskX
git push origin --delete taskX

overwrite local changes with the last commit
git reset HEAD --hard

push the local branch to remote, -u flag creates a branch
git push -u origin branchname

Changelog
git log --pretty="- %cs : %s" > CHANGELOG.md

http://udacity.github.io/git-styleguide/
commit - m "feat: new feature" -m "optional long description"

feat: A new feature
fix: A bug fix
docs: Changes to documentation
style: Formatting, missing semi colons, etc; no code change
refactor: Refactoring production code
test: Adding tests, refactoring test; no production code change
chore: Updating build tasks, package manager configs, etc; no production code change

**Azure:**
tmux
tmux detach or you can use the shortcut (Ctrl+b then d)
tmux attach
