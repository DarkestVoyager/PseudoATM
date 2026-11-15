#include "account.h"

namespace {


	std::pair<std::string, std::string> logDateAndTime()
	{
		std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm tm{};
#ifdef _WIN32
		localtime_s(&tm, &t);
#else
		localtime_r(&t, &tm);
#endif

		std::ostringstream data, ora;
		data << std::put_time(&tm, "%Y-%m-%d");
		ora << std::put_time(&tm, "%H:%M:%S");
		return { data.str(), ora.str() };
	}

	std::string lastLoggedDate(const std::string& path)
	{
		std::ifstream in{ path };
		const std::string prefix{ "=== " };
		std::string line{}, last{};

		while (std::getline(in, line))
			if (line.rfind(prefix, 0) == 0) // linia incepe cu "=== "
				last = line.substr(prefix.size(), 10); // "YYYY-MM-DD" are 10 caractere
		return last;
	}

} // namespace

Account::Account(std::string name, std::string_view pin, std::int64_t sold)
	: m_name{ toUpper(name) }, m_sold{ sold }, m_pinHash{ hashPin(pin) } {
}

Account Account::fromFile(std::string name, std::uint64_t pinHash, std::int64_t sold)
{
	Account a{ std::move(name), "", sold };
	a.m_pinHash = pinHash;
	return a;
}

bool Account::checkPIN(std::string_view pin) const
{
	return hashPin(pin) == m_pinHash;
}

bool Account::checkName(std::string_view name) const
{
	return toUpper(name) == m_name;
}

bool Account::deposit(std::int64_t suma)
{
	if (suma <= 0) return false;
	m_sold += suma;
	logTransaction("DEPUNERE", suma);
	return true;
}

bool Account::withdraw(std::int64_t suma)
{
	if (suma <= 0 || suma > m_sold) return false;
	m_sold -= suma;
	logTransaction("RETRAGERE", suma);
	return true;
}

void Account::printInfo() const
{
	std::cout << "Nume: " << m_name << '\n';
	std::cout << "Sold: " << m_sold << '\n';
}

bool Account::isValidPIN(std::string_view pin)
{
	return pin.size() == 4 &&
		std::all_of(pin.begin(), pin.end(),
			[](unsigned char c) { return std::isdigit(c); });
}

std::string Account::setPIN()
{
	std::string pin{};
	while (true) {
		std::cin >> pin;

		if (std::cin.eof()) return {};   // EOF: iesim din bucla

		if (!std::cin.fail() && isValidPIN(pin)) {
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return pin;
		}

		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cerr << "PIN invalid. Reincearca: ";
	}
}

// FNV-1a
std::uint64_t Account::hashPin(std::string_view pin)
{
	std::uint64_t h = 14695981039346656037ULL;
	for (unsigned char c : pin) {
		h ^= c;
		h *= 1099511628211ULL;
	}
	return h;
}

std::string Account::toUpper(std::string_view s)
{
	std::string out;
	out.reserve(s.size());
	for (unsigned char c : s)
		out += static_cast<char>(std::toupper(c));
	return out;
}

// "NUME PRENUME" -> "cont_NUME_PRENUME"
std::string Account::fileNameFor(std::string_view name)
{
	std::string base{ "cont_" };
	for (unsigned char c : toUpper(name))
		base += std::isalnum(c) ? static_cast<char>(c) : '_';
	return base;
}

bool Account::save() const
{
	std::ofstream out{ fileNameFor(m_name) + ".txt" };
	if (!out) return false;

	out << m_name << '\n' << m_pinHash << ' ' << m_sold << '\n';
	return static_cast<bool>(out);
}

std::optional<Account> Account::load(std::string_view name)
{
	std::ifstream in{ fileNameFor(name) + ".txt" };
	if (!in) return std::nullopt; // contul nu exista

	std::string storedName{};
	std::uint64_t hash{};
	std::int64_t sold{};

	if (!std::getline(in, storedName) || !(in >> hash >> sold))
		return std::nullopt; // fisier corupt

	return fromFile(std::move(storedName), hash, sold);
}

void Account::logTransaction(std::string_view type, std::int64_t suma) const
{
	const std::string path{ fileNameFor(m_name) + "_istoric.txt" };
	const auto [data, ora] = logDateAndTime();

	const bool zinoua{ lastLoggedDate(path) != data };

	std::ofstream out{ path, std::ios::app };
	if (!out) return;

	if (zinoua) {
		out << "=== " << data << " ===\n";
	}
	out << ora << ' ' << type << ' ' << suma << " | sold: " << m_sold << '\n';
}

void Account::printHistory() const
{
	std::ifstream in{ fileNameFor(m_name) + "_istoric.txt" };
	if (!in) {
		std::cout << "Nu exista tranzactii.\n";
		return;
	}

	std::string line{};
	while (std::getline(in, line))
		std::cout << line << '\n';
}