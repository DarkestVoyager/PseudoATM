#include "account.h"


// Citeste o suma; intoarce false daca inputul e invalid
static bool readAmount(std::int64_t& out)
{
	if (std::cin >> out) return true;

	if (std::cin.eof()) return false;
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cerr << "Suma invalida.\n";
	return false;
}

static bool login(const Account& cont)
{
	for (int attempt = 1; attempt <= 3; ++attempt) {
		std::cout << "PIN (" << attempt << "/3): ";
		std::string pin{};
		if (!(std::cin >> pin)) return false;   // EOF

		if (cont.checkPIN(pin)) return true;
		std::cerr << "PIN gresit.\n";
	}
	return false;
}

int main()
{
	std::cout << "Introduceti numele beneficiarului: ";
	std::string nume{};
	std::getline(std::cin >> std::ws, nume);

	std::optional<Account> acc{ Account::load(nume) };

	if (acc) {
		std::cout << "Cont gasit.\n";
		if (!login(*acc)) {
			std::cerr << "Cont blocat.\n";
			return 1;
		}
	}
	else {
		std::cout << "Cont nou. Alegeti un PIN (4 cifre): ";
		std::string pin{ Account::setPIN() };
		if (pin.empty()) return 1;

		acc.emplace(nume, pin, 0);
		if (!acc->save()) {
			std::cerr << "Nu s-a putut salva contul.\n";
			return 1;
		}
	}

	Account& cont{ *acc };

	int optiune{ -1 };
	while (optiune != 0) {
		std::cout << "\n1. Sold\n2. Depunere\n3. Retragere\n4. Info cont\n5. Istoric\n0. Iesire\n> ";
		if (!(std::cin >> optiune)) {
			if (std::cin.eof()) break;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			optiune = -1;
			std::cerr << "Optiune necunoscuta.\n";
			continue;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		std::int64_t suma{};
		switch (optiune) {
		case 1:
			std::cout << "Sold: " << cont.getSold() << '\n';
			break;
		case 2:
			std::cout << "Suma de depus: ";
			if (readAmount(suma) && cont.deposit(suma)) {
				cont.save();
				std::cout << "Depunere reusita.\n";
			}
			else
				std::cerr << "Depunere esuata.\n";
			break;
		case 3:
			std::cout << "Suma de retras: ";
			if (readAmount(suma) && cont.withdraw(suma)) {
				cont.save();
				std::cout << "Retragere reusita.\n";
			}
			else
				std::cerr << "Fonduri insuficiente sau suma invalida.\n";
			break;
		case 4:
			cont.printInfo();
			break;
		case 5:
			cont.printHistory();
			break;
		case 0:
			std::cout << "La revedere!\n";
			break;
		default:
			std::cerr << "Optiune necunoscuta.\n";
		}
	}
}