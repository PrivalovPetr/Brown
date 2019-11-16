#pragma once

#include <istream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <iomanip>

namespace Json {

	class Node : std::variant<std::vector<Node>,
		std::map<std::string, Node>,
		int,
		double,
		bool,
		std::string> {
	public:
		using variant::variant;

		const auto& AsArray() const {
			try {
				return std::get<std::vector<Node>>(*this);
			}
			catch (...) {
				throw std::invalid_argument("Expected array, but found something else");
			}
		}
		const auto& AsMap() const {
			try{
				return std::get<std::map<std::string, Node>>(*this);
			}
			catch (...) {
				throw std::invalid_argument("Expected Map, but found something else");
			}
		}
		
		int AsInt() const {
			try {
				return std::get<int>(*this);
			}
			catch (...) {
				throw std::invalid_argument("Expected Int, but found something else");
			}
		}
		double AsDouble() const {
			try {
				if (auto pval = std::get_if<int>(this))
					return *pval * 1.0;
				return std::get<double>(*this);
			} catch (...) {
				throw std::invalid_argument("Expected Double or int, but found something else");
			}
		}
		bool AsBoolean() const {
			try{
				return std::get<bool>(*this);
			}
			catch (...) {
				throw std::invalid_argument("Expected Boolean, but found something else");
			}
		}
		const auto& AsString() const {
			try {
				return std::get<std::string>(*this);
			}
			catch (...) {
				throw std::invalid_argument("Expected string, but found something else");
			}
		}

		void Print(std::ostream& os, std::string& offset) const {
			if (auto pval = std::get_if<std::vector<Node>>(this)) {
				if (pval->empty())
					os << "[]";
				else {
					os << "[\n";
					std::string new_offset = offset + "  ";
					for (int i = 0; i < pval->size(); ++i) {
						if (i > 0)
							os << ",\n";
						os << new_offset;
						(*pval)[i].Print(os, new_offset);
					}
					os << "\n" << offset << "]";
				}
			}
			else if (auto pval = std::get_if<std::map<std::string, Node>>(this)) {
				if (pval->empty())
					os << "{}";
				else {
					os << "{\n";
					std::string new_offset = offset + "  ";
					bool first = true;
					for (const auto& item : *pval) {
						if (!first)
							os << ",\n";
						os << new_offset << '"' << item.first << "\": ";
						item.second.Print(os, new_offset);
						first = false;
					}
					os << "\n" << offset << "}";
				}
			} else if (auto pval = std::get_if<int>(this)) {
				os << *pval;
			} else if (auto pval = std::get_if<double>(this)) {
				os << std::setprecision(6) << *pval;
			} else if (auto pval = std::get_if<bool>(this)) {
				if (*pval)
					os << "true";
				else
					os << "false";
			} else if (auto pval = std::get_if<std::string>(this)) {
				os << "\"" << *pval << "\"";
			}
		}
	};

	class Document {
	public:
		explicit Document(Node root);

		const Node& GetRoot() const;

	private:
		Node root;
	};

	Document Load(std::istream& input);

}
