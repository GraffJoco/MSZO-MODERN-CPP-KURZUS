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
Ezért van C++20 óta a `ranges` könyvtár (amit C++23-ban jelentősen bővítettek, és még ennél is többet fognak).

# Ranges, views, funkcionális C++

## Alapok: iterátor, range, view

Ahhoz, hogy a `<ranges>` által adott lehetőségeket kihasználjuk, meg kell ismerni pár alapfogalmat.  

### Iterátor

Ha MOO-ból szóbeliztél már, akkor ezt a fogalmat biztos hallottad, és esély szerint nagyon összezavarodtál miatta. Az iterátor nem egy nehéz fogalom, de *nehezen magyarázható*.  
Az iterátorok a következők: minden féle STL tárolónak van egy kis segédosztálya, amivel egyesével az elemei bejárhatóak. Ezek az iterátorok. Amikor végig mész egy iteráló forral (pl.: `for (auto i : vec)`), akkor először az elején a program kér a tárolótól egy kezdőiterátort (`it`), minden ciklusban veszi az aktív elemét (`auto i = *it;`), és a végén növeli az iterátor pozíciót (`it++;`) amíg nem érünk a végére (`it != vec.end()`). Az egészet kifejtve egy iterátoros for ciklus a következő módon néz ki:

~~~C++
int main() {
    std::vector adatok = { 1, -5, 9, -420, 3 };
    
    for (auto it = adatok.begin(); it != adatok.end(); it++) {
        const auto i = *it;

        std::println("{}", i);
    }

    // Ugyanaz, mint

    for (auto i : adatok) {
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
