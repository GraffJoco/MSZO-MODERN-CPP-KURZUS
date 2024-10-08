# Tartalom

- [BEVEZETÉS](#bevezetés)
  - [LAMBDA KALKULUS ALAPJAI](#lambda-kalkulus-alapjai)
  - [TISZTA FÜGGVÉNYEK, MUTABILITÁS](#tiszta-függvények-mutabilitás)
- [LAMBDÁK, ALGORITMUSOK A C++-BAN](#lambdák-algoritmusok-a-c-ban)
  - [TISZTA LAMBDA FÜGGVÉNYEK (C++11)](#tiszta-lambda-függvények)
  - [STL ALGORITMUSOK (C++11)](#stl-algoritmusok)
- [RANGES, VIEWS, FUNKCIONÁLIS C++ (C++20/C++23)](#ranges-views-funkcionális-c)
  - [ITERÁTOR, RANGE, VIEW](#alapok-iterátor-range-view)
    - [ITERÁTOR](#iterátor)
    - [RANGE](#range)
    - [VIEW](#view)
  - [FUNKCIONÁL PROGRAMOZÁS A `<RANGES>` KÖNYVTÁRRAL](#funkcionál-programozás-alapjai-a-ranges-könyvtárral)
    - [STRINGDARABOLÁS SAJÁT SPLIT FÜGGVÉNNYEL](#stringdarabolás-saját-split-függvénnyel)

# Bevezetés

A funkcionál programozás nagyon más, mint az imperatív vagy akár objektum orientált programozás, ezért sajnos itt egy elméleti bevezető is kelleni fog.

## Lambda kalkulus alapjai

A funkcionál programozás az ú.n. lambda kalkulusra épül, aminek a következő a lényege: *minden* matematikai fogalom kifejezhető egyváltozós függvények kombinálásával. Ez a szoftveres világban olyan szinten appikálható, hogy minden program is leírható függvények kreatív kombinálásával.

Lambda kalkulusban a függvényeket egy $\lambda$-val jelölik, utána a paramétereket, pont után pedig a kifejezést:

$$ E \equiv \lambda x .x+5$$  

$$ E(5) \equiv 10 $$

Amennyiben többváltozós függvényünk van, körrizést felhasználva tudunk írni egy olyan egyváltozós függvényt, ami egy másik egyváltozós függvényt ad vissza (>2 paraméternél ezt többször megismételve) egy paramétert behelyettesítve:

$$ F \equiv \lambda x.\lambda y.x+y$$  

$$ F(5)(7) \equiv (\lambda x. \lambda y. x + y)(5)(7) \equiv (\lambda y . 5 + y)(7) \equiv 5 + 7 = 12 $$

C++-ban ez a következő módon nézhet ki:

~~~C++
const auto F = [](const int x) {
    return [=](const int y) {
        return x + y;
    };
};
~~~

Habár C++-ban a körrizést szerencsére nem kell használni (más nyelvekben, pl.: Haskellben kötelező), ebből egy olyan fontos következtetést lehet levonni, amire szükségünk lesz: **első osztályúak a függvények, tehát paraméterként, értékként és függvények eredményeként is kezelhetőek**. Aki már használt C++ lambdákat (amik innen kapták a nevüket) az `<algorithm>` könyvtárral STL tárolókon, az már ezt a tulajdonságot kihasználta.

## Tiszta függvények, mutabilitás

A funk. prog. egyik legfontosabb fogalma, és alapja a tiszta függvények használata. A tiszta függvények a következő szabályoknak felelnek meg:

- A függvény kimenete csak a paraméterek ismeretével mindig meghatározható (nem olvas külső értékeket a paraméterein kívül)
- Nem módosít semmilyen bemeneti paramétert
- Nincsenek mellékhatásai (nem nyúl globális/külső változókhoz, radikális esetben még I/O művelet is tiltott)

Ha egy függvény tiszta, akkor egyszerű teszteléssel lehet garantálni, hogy stabilan, és mindig úgy csinálja feladatát, ahogyan azt definiáltuk. Természtesen bizonyos bemenetekre vigyázni kell, hogy a lehetséges hibákat, error értékeket detektáljuk, és lekezeljük.  
Amennyiben tud egy függvény sikertelen lenni, error értéket kell visszaadni eredményként, hogy a hívó tudja, hogy a függvény nem mindig sikeres, és lekezelje ezt az esetet. Erre volt hasznos az első alkalomról a `std::optional<T>` típusunk, és ennek a bővítése, a `std::expected<T, U>`, amiben konkrét adatként tudjuk pontosítani az errort.

A hibák elkerülésére a "mutabilis" értékeket is ajánlott minimalizálni. Magyarra fordítva, próbálj mindent konstansokkal megoldani, mert tudod, ha a kezdeti értéke jó, akkor mindenhol is az. Emiatt már egyes nyelvekben (pl.: Rustban) azt kell külön kulcsszóval jelölni, ha *nem* konstans a változó, és van, ahol csak konstansok léteznek (pl.: Haskell, Gleam). Ez természetesen nem a legjobb megoldás, gyakran kell, hogy a változók értéke *változzon*, de ha lehet, ajánlott konstansokat használni.

# Lambdák, algoritmusok a C++-ban

Ahhoz, hogy a C++-ban tudjuk ezt a tudást alkalmazni, elő kell hozni az [első alkalom](../Alkalom%201/anyag.md) alatt tanult lambdákat.  
A lambda egy funktor (függvényként használható osztály), amit szabadon akárhol létrehozhatunk a következő szintaxissal:

~~~C++
const auto lamdba = [/*megfogott értékek*/](/*paraméterek*/) -> /*visszaadott érték típusa (nem kötelező)*/ {
    // függvény elemek
};
~~~

## Tiszta lambda függvények

Ahhoz, hogy tiszta függvényként kezelhessük a lambdánkat, a következő szabályoknak kell megfelelnie:

- Minden változót vagy `=`-vel érték szerint másolunk, vagy ha `&`-vel referenciaként másoljuk, akkor nem módosítjuk értékét, de optimális esetben **nem fogunk meg külső értéket, csak paraméterként**
- A paraméterek vagy sima értékek vagy konstans referenciák

Például:

~~~C++
#include <print>

int main() {
    int val = 5;
    
    // Tiszta fv
    const auto duplaz1 = [](const auto param) {
        return param * 2;
    };

    // Tiszta fv
    const auto duplaz2 = [](const auto& param) {
        return param * 2;
    };

    // Nem tiszta, paramétert módosítja
    const auto duplaz3 = [](auto& param) {
        param *= 2;
    };

    // Nem tiszta, külső értéket mutál
    const auto duplaz4 = [&val]() {
        val *= 2;
    };

    // Nem tiszta, de jobb, mint az előző, mert val nem módosul
    const auto duplaz5 = [=]() {
        return val * 2;
    };

    // Technikailag nem tiszta, de konzolba kiírás nem nagy probléma
    const auto duplaz6 = [](const auto& param) {
        const auto ret = param * 2;
        std::println("{}", ret);
        return ret;
    };
}
~~~

## STL Algoritmusok

A C++ `<algorithm>` könyvtárával lambdákkal előre elkészített, performans algoritmusokat lehet hívni STL tárolókra.

Például lehet egy szabály alapján törölni elemeket egy tárolóból:

~~~C++
#include <algorithm>
#include <print>
#include <vector>

int main() {
    std::vector adatok = { 1, -5, 9, -420, 3 };
    // törli az elemeket, amik ennek megfelelnek
    std::erase_if(adatok, [](const auto ertek) {
        return ertek < 0;
    });

    std::println("{}", adatok);
}
~~~

Nagyon sok fajta algoritmus van implementálva:

- `std::for_each` egy lambdát hív a tároló minden elemén
- `std::transform` átalakítja egy tároló elemeit egy lamdba alapján
- `std::find_if` megkeres egy elemet, ami a lambdának megfelel
- `std::erase_if` elemeket töröl egy lambda szerint (C++20)
- `std::remove_if` elemeket töröl, de a tároló méretét nem csökkenti (ahhoz extra kód kell)
- `std::sort` rendezi az elemeket
- [és még sok, sok más is](https://en.cppreference.com/w/cpp/algorithm)

Ez egy relatíve jó megoldás, és sokat segít, de ha több algoritmust akarsz kombinálni, eléggé szuboptimális, valamint ezek az algoritmusok gyakran nem tiszták, amiből baj lehet.  
Ezért van C++20 óta a `<ranges>` könyvtár (amit C++23-ban jelentősen bővítettek, és még ennél is többet fognak).

# Ranges, views, funkcionális C++

## Alapok: iterátor, range, view

Ahhoz, hogy a `<ranges>` által adott lehetőségeket kihasználjuk, meg kell ismerni pár alapfogalmat.  

### Iterátor

Ha MOO-ból szóbeliztél már, akkor ezt a fogalmat biztos hallottad, és esély szerint nagyon összezavarodtál miatta. Az iterátor nem egy nehéz fogalom, de *nehezen magyarázható*.  
Az iterátorok a következők: minden féle STL tárolónak van egy kis segédosztálya, amivel egyesével az elemei bejárhatóak. Ezek az iterátorok.  
Például amikor végig mész egy iteráló forral (`for (auto i : vec)`), akkor először az elején a program kér a tárolótól egy kezdőiterátort (`it`), minden ciklusban veszi az aktív elemét (`auto i = *it;`), és a végén növeli az iterátor pozíciót (`it++;`) amíg nem érünk a végére (`it != vec.end()`). Az egészet kifejtve egy iterátoros for ciklus a következő módon néz ki:

~~~C++
int main() {
    std::vector adatok = { 1, -5, 9, -420, 3 };
    
    for (auto it = adatok.begin(); it != adatok.end(); it++) {
        const auto i = *it;

        std::println("{}", i);
    }

    // Ugyanaz, mint

    for (const auto i : adatok) {
        std::println("{}", i);
    }
}
~~~

Tehát az iterátor a következőket tudja:

- `tarolo.begin()`: visszaadja az első elem pozíciójának iterátorát
- `tarolo.end()`: visszaadja az iterátort, ami a tároló végét jelöli
- `*it`: visszaadja az iterátor pozíción lévő elemet
- `it++`: megy a következő elemre

Ez hasonló a pointeres tömbbejáráshoz még IPA-ról, egy különbséggel: mivel az iterátor osztály, a `++` operátor nem feltétlenül a memóriában ezutáni blokkra fog mutatni, tehát **nem memóriában lineárisan tárolt adatokat is be lehet járni** vele.

### Range

A range egy intervallumot fed le egy kezdő és egy végiterátor között: pl.: `adatok` vektornál $[$ `adatok.begin()`; `adatok.end` $[$ egy range. A rangek egy speciális tulajdonsága, hogy lehet speciális rangeket létrehozni új adatok allokálása nélkül: ha van egy $a$ STL tárolónk, tudunk csinálni egy iterátort, ami valamilyen $B$ szabály alapján átugrik pár elemet, tehát extra memória foglalása nélkül tudunk csinálni $a$-ból egy új $b$ range-t, ami $a$ alhalmaza. Ha $a$ konstans, akkor $b$ is, tehát memóriahiba miatt nem kell aggódni.

### View

A view kombinálja az iterátorokat és rangeket: lehet olyan speciális iterátorokat gyártani, amik nem csak az eredeti tárolónak egy alhalmazán megy át, de a bejárási sorrend és irány változtatható, sőt, leképező függvénnyel az aktív elemet át lehet alakítani. Ezt az egészet csak pár byte memóriáért cserébe képesek csinálni. Emellé ha egy konstans tárolón tiszta függvényekkel csináljuk az átalakításunkat, stabilan tudjuk ezeket mind megtenni.

## Funkcionál programozás alapjai a `<ranges>` könyvtárral

Most, hogy megértettünk minden szükséges fogalmat, ideje a `<ranges>` könyvtárat használni is.  
A könyvtárral rangekből viewokat lehet csinálni azzal, hogy a könyvtár egyes utasításait egymás után hívjuk a `|` operátorral elválasztva:

~~~C++
#include <algorithm>
#include <vector>
#include <ranges>
#include <print>

int main() {
    // sok namespace egymásban, így egyszerűbb elérni
    namespace ranges = std::ranges;
    namespace views = std::ranges::views;

    const std::vector vals = { 1, -2, 5, 9, -8, -420 };

    // új viewt csinálunk:
    // 1. vesszük vals elemeit
    // 2. views::filterrel kiszűrjük a pozitív értékeket
    // 3. views::transformmal definiálunk egy átalakító fv-t
    //    (vals-t nem módosítja)
    auto atalakitva = vals
        | views::filter([](const auto val) { return val > 0; })
        | views::transform([](const auto val) { return val * val; });

    for (const auto val : atalakitva) {
        std::println("{}", val);
    }

    std::println("Ennek az egesznek a memoriaigenye {} byte volt", sizeof(atalakitva));
}
~~~

Ezt futtatva látjuk, hogy a kód működik, és az átalakító view csak 32 byte memóriát foglal:

![Első ranges demonstráció](kepek/rangeproba.png)

Azért kell neki ilyen kevés memória, mert valójában csak `vals` helyét és az egyes leképező függvényeket kell ismernie. Ez a méret függvetlen attól, hogy hány elemünk van, csak a műveletek száma tudja növelni, de akkor is elhanyagolható marad.

[A könyvtár számtalan műveletet, átalakítást és egyéb eszközt tartalmaz](https://en.cppreference.com/w/cpp/ranges), ezért itt csak párat fogok demonstrálni a legjobbakból.

### Stringdarabolás saját split függvénnyel

Az egyik dolog, ami minden fejlesztőnek hiányzik, aki magasabb szintű nyelvből jött, egy `split` függvény: adsz neki egy stringet, és hogy milyen elválasztó alapján daraboljon, és visszaad neked egy listát/tömböt a darabolt string elemeivel. Ezt a C++ alapból nem tudja, de pár sor kóddal tudjuk magunknak implementálni.  

A következő módon tudjuk ezt implementálni:

1. Csinálunk egy `split` függvényt, ami kér egy `std::string_view` szöveget darabolásra és egy elválasztó karaktert
2. A `views::split` függvénnyel kapunk egy csomó range-t, ami tartalmazza az intervallumokat, amikből tudjuk az új stringeket létrehozni
3. Egy `views::transform` függvénnyel a range-kből tudunk rendes stringeket csinálni
4. A `ranges::to<T>` függvénnyel vektort csinálunk a string rangeből

Gyakorlatban ez a következő módon néz ki:

~~~C++
#include <algorithm>
#include <vector>
#include <string>
#include <ranges>
#include <print>

namespace ranges = std::ranges;
namespace views = ranges::views;

std::vector<std::string> split(const std::string_view from, const char delim) {
    return from
        | views::split(delim)
        | views::transform([](const auto strrange) {
            return std::string(strrange.begin(), strrange.end());
        })
        | ranges::to<std::vector>();
}

// Azonnal tesztelhetjük is, hogy minden működik-e
int main() {
    const std::string teszt = "Ez;Egy;Teszt;String";
    const auto splitted = split(teszt, ';');

    for (const auto& str : splitted) {
        std::println("{}", str);
    }
}
~~~

### Súlyfüggvények bővülő adatsorokon

Mechatronikában gyakori, hogy az utolsó $n$ mért pontra egy adott súlyfüggvényt kell rakni. Ezt az is tudja programozásban komplikálni, hogy az adatsorunk egy szabályzási folyamat során valós időben bővül úgy, hogy közben ebből az adatsorból azonnal egy súlyfüggvénnyel kell csinálnunk valamilyen kimeneti értéket.

Ilyen esetekben kis problémát okozhatna az, hogy ha ezt funkcionálisan akarjuk leprogramozni, egyszerre két adatsoron kell átmennünk. Erre használható a `views::zip`, ami egy olyan view-t ad nekünk, aminek minden eleme egy tuple, ami tartalmazza az $n.$ értékét minden megadott view-nak. A visszakapott view mérete a legkisebb méret minden viewból, tehát nem lehet ebből kicímzés sem.

Tehát a következőket kell elérni egy rendes súlyfüggvény létrehozásához:

1. A bemeneteket (érték és súlyvektor) bezippelni, de az értékvektor view-jának sorrendjét először megfordítjuk, mert az utolsó $n$ érték kell nekünk
2. A keletkezett tuple elemeit összeszorozzuk
3. Az így létrehozott view elemeit szummázzuk a `ranges::fold_left` függvénnyel, és visszaadjuk az eredményt

Így a következő módon nézhet ki egy funkcionális súlyfüggvény:

~~~C++
#include <algorithm>
#include <vector>
#include <string>
#include <ranges>
#include <print>

namespace ranges = std::ranges;
namespace views = ranges::views;

double sulyozott(const std::vector<double>& ertekek, const std::vector<double>& sulyok) {
    // súlyozott view létrehozása
    const auto sulyertekek = views::zip(ertekek | views::reverse, sulyok)
        | views::transform([](const std::tuple<double, double>& val) {
            return std::get<0>(val) * std::get<1>(val);
        });

    // szumma kiszámolása
    return ranges::fold_left(sulyertekek, 0.0, [](const double a, const double b) {
        return a + b;
    });
}

int main() {
    const std::vector sulyok = { 2.0, -0.9, 0.5, 0.1 };
    std::vector<double> ertekek;

    // Bemenet: u[k] = epsilon[k]
    for (int i = -2; i < 5; i++) {
        ertekek.push_back(i >= 0 ? 1.0 : 0.0);

        std::println("{:2}: {:.3}", i, sulyozott(ertekek, sulyok));
    }
}
~~~
