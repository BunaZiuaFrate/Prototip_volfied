#ifndef GAMEEXCEPTIONS_H
#define GAMEEXCEPTIONS_H

#include <exception>
#include <string>

/// @brief Clasa de baza pentru exceptiile jocului.
class GameException : public std::exception {
protected:
    std::string message;
public:
    explicit GameException(const std::string& msg) : message(msg) {}
    // [FEEDBACK] noexcept si const
    const char* what() const noexcept override { return message.c_str(); }
};

/// @brief Exceptie aruncata cand o entitate iese din harta.
class OutOfBoundsException : public GameException {
public:
    explicit OutOfBoundsException(const std::string& name)
        : GameException("CRITIC: " + name + " a incercat sa iasa din harta!") {}
};

/// @brief Exceptie pentru comenzi invalide de la tastatura.
class InvalidActionException : public GameException {
public:
    InvalidActionException() : GameException("Actiune invalida! Comanda necunoscuta.") {}
};

#endif //GAMEEXCEPTIONS_H