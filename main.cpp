#include <iostream>
#include <array>
#include <vector>

class Level;

void *operator new (size_t size){
    void *p = malloc(size);
    if (!p){ std::bad_alloc ba; throw ba;}
    return p;
}

void operator delete(void *p){ free(p);}

struct Position {
    int x;
    int y;
};

class Player {
    bool collision;
    int speed;
    char color;
    Position position;

    public:
    Player() {
        collision = true;
        position.x = 0;
        position.y = 0;
    }

    Player(int speed, char color) {
        collision = true;
        position.x = 0;
        position.y = 0;
        this->speed = speed;
        this->color = color;
    }

    int getspeed(int speed) const {
        return speed;
    }
    void setspeed(int speed) {
        this->speed = speed;
    }

    Player(const Player &obiect_de_copiat) {
        collision=obiect_de_copiat.collision;
        speed = obiect_de_copiat.speed;
        color = obiect_de_copiat.color;
    }

    friend std::ostream& operator<<(std::ostream &os, const Player & X) {
        os << X.color << X.speed << X.color;
        return os;
    }

    friend bool Collision_finder(Position x, Level y);
};

class Score {
    int level_score, global_score;

    public:
    Score() {
        level_score = global_score = 0;
    }

    Score( int global_score) {
        this->global_score = global_score;
    }

    void addscore(int level_score) {
        global_score += level_score;
    }

    Score(const Score &scor_vechi) {
        level_score = scor_vechi.level_score;
        global_score = scor_vechi.global_score;
    }

    Score& operator=(Score& scor_vechi);

    friend std::ostream& operator<<(std::ostream &os, const Score & X) {
        os << X.level_score << X.global_score;
        return os;
    }

    ~Score(){}

    void Calculate_score(int &global_score, int level_score) {
        global_score += level_score;
    }
    void Reset_score() {
        level_score = 0;
    }
};

Score& Score::operator=(Score& scor_vechi) {
    level_score=scor_vechi.level_score;
    global_score=scor_vechi.global_score;
    return *this;
}

class Level {
    int level_number, difficulty;

    public:
    Level() {}
    Level(int level_number) {
        this->level_number = level_number;
    }

    friend std::ostream& operator<<(std::ostream &os, const Level& X) {
        os << X.level_number << X.difficulty;
        return os;
    }

    friend bool Collision_finder(Position x, Level y);

};

class GameState {
    Player mainPlayer;
    Score currentScore;
    Level currentLevel;

    public:
    GameState(const Player& p, const Score& s, const Level& l)
        : mainPlayer(p), currentScore(s), currentLevel(l) {}

    friend std::ostream& operator<<(std::ostream& os, const GameState& gs);
};

std::ostream& operator<<(std::ostream& os, const GameState& gs) {
    os << " Starea Jocului" << std::endl;
    os << " Jucator: " << gs.mainPlayer << std::endl;
    os << " Scorul:  " << gs.currentScore << std::endl;
    os << " Nivelul: " << gs.currentLevel;
    os << std::endl << "----------------------";
    return os;
}



void Update() {

}

int main() {
    Level *current_level=nullptr;
    Score *current_score=nullptr;
    for (int level=1; level<=9; level++) {
        try {current_score = new Score();}
        catch (std::bad_alloc xa) {
            std::cout<<"Problema la alocare";
            return -1;
        }
        try {current_level = new Level(level);}
        catch (std::bad_alloc xa){
            std::cout<<"Problema la alocare";
            return -1;
            }
        current_score->addscore(level);
        current_score->Reset_score();
        delete current_level;
        delete current_score;
        }
        /////////////////////////////////////////////////////////////////////////
        /// Observație: dacă aveți nevoie să citiți date de intrare de la tastatură,
        /// dați exemple de date de intrare folosind fișierul tastatura.txt
        /// Trebuie să aveți în fișierul tastatura.txt suficiente date de intrare
        /// (în formatul impus de voi) astfel încât execuția programului să se încheie.
        /// De asemenea, trebuie să adăugați în acest fișier date de intrare
        /// pentru cât mai multe ramuri de execuție.
        /// Dorim să facem acest lucru pentru a automatiza testarea codului, fără să
        /// mai pierdem timp de fiecare dată să introducem de la zero aceleași date de intrare.
        ///
        /// Pe GitHub Actions (bife), fișierul tastatura.txt este folosit
        /// pentru a simula date introduse de la tastatură.
        /// Bifele verifică dacă programul are erori de compilare, erori de memorie și memory leaks.
        ///
        /// Dacă nu puneți în tastatura.txt suficiente date de intrare, îmi rezerv dreptul să vă
        /// testez codul cu ce date de intrare am chef și să nu pun notă dacă găsesc vreun bug.
        /// Impun această cerință ca să învățați să faceți un demo și să arătați părțile din
        /// program care merg (și să le evitați pe cele care nu merg).
        ///
        /////////////////////////////////////////////////////////////////////////


        ///////////////////////////////////////////////////////////////////////////
        /// Pentru date citite din fișier, NU folosiți tastatura.txt. Creați-vă voi
        /// alt fișier propriu cu ce alt nume doriți.
        /// Exemplu:
        /// std::ifstream fis("date.txt");
        /// for(int i = 0; i < nr2; ++i)
        ///     fis >> v2[i];
        ///
        ///////////////////////////////////////////////////////////////////////////
    return 0;
}
