Tests:
// ToDo odstinit sito od mereni, pocitat v cyklu ??
// generovat nahodne ulohy a porovnavat casy a vysledky
// test sita od 0 do 2
// test sita u 2 na 64
// test 128 pres 2 na 64 a u konce - new work template
// print ext progress - vypocet casu dl zpracovane casti vlaknem
// nacitani threads a prdcasny exit nekterych vlaken, co hodnota 0?
// lepsi vypocet switch pointu dle aktuální velocity???
corner case checker 0,0 1,1, 2,2 3,3 0,19, 0, 21, primorial, u 64 na sieve 128
- test na sieve u konce hranice 2 na 128  a testy overflow
- test n sieve delky 2
- test na sieve, kde je end = (k * primorial) -1
begin > end
attribut processed ok na delku, overflow?, 
prejmenovat to process
performance clang

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
- CLang warnings a performance
- test kde beg > end
- kontrola bits 64 65 128
- projit ??? a overflow
- dev param do make
- optimalizovane testy BPSW ??
- Todo: nefunční web prime17 při startu spadne do registrace. Testy, zda funguje prime17 web a zda je config file naplnený
- co je nejrychlesji test na oblast 48-52
- doplnit asseert
- doplnit const
- Podívat se na virtuální destruktory a rule of the three or five
- vylepšit try catch
- cmake, autoconf?
- versioning
- PrimeStorage  - použít config dir
- prejít na c++20? a nové threads se zastavováním a moduly
- use cpp wrapper for ncurses? https://stackoverflow.com/questions/53857540/using-native-c-wrappers-for-ncurses-how-can-i-create-a-menu-submenu-system
- ARM version - odstranit vsechny long promenne a nahradit je uint64
- ncurses resize bugs: seg fault, malloc(): unsorted double linked list corrupted
- see - https://invisible-island.net/ncurses/ncurses.faq.html#handle_resize
- see https://stackoverflow.com/questions/14897157/what-does-corrupted-double-linked-list-mean
- ncurses resize 0 bug - also in Midnight Commander in WSL
- ncurses - resize cause reading an input key
- sanitizer screams various ugly errors when resizing via handle_winch is set 
- sanitizer options must be set in a enviroment variable: LSAN_OPTION=verbosity=1:logThreads_=1


ToDo Web:
- FAQ
- vyuzivat includy a prepsat stare all in one pages
- https://oeis.org/search?q=2%2C2%2C3%2C3%2C3%2C2%2C3%2C4%2C4&sort=&language=czech&go=Hledej

ToDo DB:
- vymyslet tabulky pro testování- druha db?
- opravit prime pi reconciliation https://oeis.org/A036378
- vstup jako Json?
- výpočet super slices
- work_results_rec  - promazavání
- formatovat cisla 000 000??
- time_zones +2 jen v completed
- FNj pridat desetinna mista
- smazat workery a usery bez odevzdane prace a meritu, cross check merrit a work
- podporit testy tak, ze nedokoncenou praci vraci stejnemu uzivateli a nenacina zbytecne novou.


ToDo:
- Google cloud
- rezy U joining TT slices

Tests:
- reentrantnost aprcl? statické proměnné? ověřit APRCL na více vláknech
- libcurl err 6  - vypis, sleepem

Issues:
- implementace v header?
- dve instance tridy webs - todo test
- libcurl global_init není thread safe
- statický linking gmp a chyba v gcc
- Libcurl cmd test: curl https://prime17.000webhostapp.com/completed.php -v -o internet_test.txt
- wizard - nejprve se pta na threads??
- AWS vykon

Links:
https://prime17.000webhostapp.com/
https://stackoverflow.com/questions/4064444/returning-json-from-a-php-script

Reading:
https://isocpp.org/wiki/faq/exceptions#ctors-can-throw
https://stackoverflow.com/questions/45836650/how-do-i-integrate-msys2-shell-into-visual-studio-code-on-window


DB refresh:
delete from completed;
delete from worker_merit;
delete from work_results;
delete from work_results_rec;
delete from work;
delete from worker;
delete from user;
