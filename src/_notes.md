
ToDo Gmp:
- vlastní překlad gmp s bechmarkem 
- optimalizace pro miller rabina a násobky d -- jiný průchod prostorem???
- zkusit volat millera rabin a selfridge z mpz knihovny a zmerit cas. jak je mozne, ze nepocitaji jacobiho?
- zapnout v mpz optimalizaci, ze bpsw je spolehliva do 2na64
- podivat se na repetetive square misto powm
- porovnat implementaci rabina s rabinem v mpz - cast powm a jacobi
- podivat se na kod posledni verze gmp 6.2 knihovny - optimalizace powm poro base 2
- zjistit, v cem je leppsi mpfr knihovna a zda neobsahuje lepsi kody

ToDo Cpp: 
- doplnit const
- Podívat se na virtuální destruktory a rule of the three or five
- vylepšit try catch
- installer
- packaging
- cmake, autoconf?
- versioning
- PrimeStorage  - použít config dir
- prejít na c++20? a nové threads se zastavováním
- use cpp wrapper for ncurses? 
- see https://stackoverflow.com/questions/53857540/using-native-c-wrappers-for-ncurses-how-can-i-create-a-menu-submenu-system
- libcurl se nesmi zabit, jinak nefunguje spojeni?? Cistit resources??
- resize
- backspace
- deadlock - file
- nezobrazovat email - wizard, procedura
- check register zorazi mail worker
-  
- test deadlock a update completed
- dve instance tridy webs
- flushovani do souboru na win je podivne
- warningy na static unused
- opravit primorial pro release

ToDo Web:
- FAQ
- graf s výsledky?

ToDo DB:
- vymyslet tabulky pro testování
- konfigurace chunks
- vstup jako Json?
- výpočet spojených slices



Tests:
- reentrantnost aprcl? statické proměnné? ověřit APRCL na více vláknech
- zátěžový test z více počítačů

Issues:
- libcurl global_init není thread safe
- statický linking gmp a chyba v gcc
- Libcurl: curl https://prime17.000webhostapp.com/completed.php -v -o internet_test.txt

Links:
https://prime17.000webhostapp.com/
https://stackoverflow.com/questions/4064444/returning-json-from-a-php-script


DB:
delete from completed;
delete from worker_merit;
delete from work_results;
delete from work;
delete from worker;
delete from user;
