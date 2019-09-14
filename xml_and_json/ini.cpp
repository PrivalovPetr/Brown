#include "ini.h"
#include <istream>
namespace Ini {
	using namespace std;
	//using Section = unordered_map<string, string>;
	//unordered_map<string, Section> sections;
	Section& Document::AddSection(string name) {
		sections.insert({ name, Section() });
		return sections[name];
	}
	const Section& Document::GetSection(const string& name) const {
		return sections.at(name);
	}
	size_t Document::SectionCount() const {
		return sections.size();
	}
	Document Load(istream& input) {
		Document doc;
		Section*  sec = nullptr;
		for (string line; getline(input, line);) {
			if (line[0] == '[' && line[line.size() - 1] == ']') // new section
			{
				sec = &doc.AddSection(line.substr(1, line.size() - 2));
				continue;
			}
			if (line.empty()) {
				continue;
			}
			if (sec) {
				size_t eq = line.find('=');
				string k = line.substr(0, eq);
				string v = line.substr(eq + 1);
				sec->insert({ k, v });
			}
		}
		return doc;
	}
}
