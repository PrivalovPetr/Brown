#include "json.h"

using namespace std;

namespace Json {

  Document::Document(Node root) : root(move(root)) {
  }

  const Node& Document::GetRoot() const {
    return root;
  }

  Node LoadNode(istream& input);

  Node LoadArray(istream& input) {
    vector<Node> result;

    for (char c; input >> c && c != ']'; ) {
      if (c != ',') {
        input.putback(c);
      }
      result.push_back(LoadNode(input));
    }

    return Node(move(result));
  }

  Node LoadNumber(istream& input) {
	  int digit = 1;
	  char c;
	  input >> c;
	  if (c == '-')
		  digit = -1;
	  else
		  input.putback(c);
	  double result = 0;
	  while (isdigit(input.peek())) {
		  result *= 10;
		  result += input.get() - '0';
	  }
	  
	  
	  input >> c;
	  if (c == '.') {
		  double div = 10;
		  while (isdigit(input.peek())) {
			  result += double(input.get() - '0') / div;
			  div *= 10;
		  }
	  }
	  else {
		  input.putback(c);
		  int n = digit * int(result);
		  return Node(n);
	  }
	  return Node(result * digit);
  }

  /*Node LoadInt(istream& input) {
    int result = 0;
    while (isdigit(input.peek())) {
      result *= 10;
      result += input.get() - '0';
    }
    return Node(result);
  }*/

  Node LoadString(istream& input) {
    string line;
    getline(input, line, '"');
    return Node(move(line));
  }

  Node LoadBoolean(bool b) {
	  return Node(b);
  }

  Node LoadDict(istream& input) {
    map<string, Node> result;

    for (char c; input >> c && c != '}'; ) {
      if (c == ',') {
        input >> c;
      }

      string key = LoadString(input).AsString();
      input >> c;
      result.emplace(move(key), LoadNode(input));
    }

    return Node(move(result));
  }

  Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
      return LoadArray(input);
    } else if (c == '{') {
      return LoadDict(input);
    } else if (c == '"') {
      return LoadString(input);
	}
	else if (c == 't') {
	  input.ignore(3);
	  return LoadBoolean(true);
	}
	else if (c == 'f'){
	  input.ignore(4);
	  return LoadBoolean(false);
	} else {
      input.putback(c);
      return LoadNumber(input);
    }
  }

  Document Load(istream& input) {
    return Document{LoadNode(input)};
  }

}
