#include <memory>
#include <string>
#include <print>

void feldolgoz(std::shared_ptr<std::string> adat) {
    std::println("{}", *adat);
}

void bovit(std::shared_ptr<std::string> adat, const std::string& mivel) {
    *adat += mivel;
}

int main() {
    auto str = std::make_shared<std::string>("Lol");

    // Ide-oda dobáljuk str-t, nem egyértelmű (a programnak)
    // hogy hol szabadul fel
    feldolgoz(str);
    
    for (auto i = 0; i < 10; i++) {
        // ugyanaz a memóriaterület, de nem kell aggódni
        // rosszul időzített delete miatt
        auto most = str;
        bovit(most, "HI");
    }

    feldolgoz(str);

    // Itt szabadul csak fel
    // Konzol:
    /*
    Lol
    LolHIHIHIHIHIHIHIHIHIHI
    */
}