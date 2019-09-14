#include "Common.h"

using ExpressionPtr = std::unique_ptr<Expression>;

class NumExpr : public Expression {
private:
	const int v;
public:

	explicit NumExpr(int value) : v(value) {}
	int Evaluate() const {
		return v;
	}
	std::string ToString() const {
		return std::to_string(v);
	}
};

class SumExpr : public Expression {
private:
	ExpressionPtr l, r;
public:
	explicit SumExpr(ExpressionPtr left, ExpressionPtr right) : l(std::move(left)), r(std::move(right)) {}
	int Evaluate() const {
		return l->Evaluate() + r->Evaluate();
	}
	std::string ToString() const {
		return "(" + l->ToString()  + ")" + "+" + "(" + r->ToString() + ")";
	}
};

class ProductExpr : public Expression {
private:
	ExpressionPtr l, r;
public:
	explicit ProductExpr(ExpressionPtr left, ExpressionPtr right) : l(std::move(left)), r(std::move(right)) {}
	int Evaluate() const {
		return l->Evaluate() * r->Evaluate();
	}
	std::string ToString() const {
		return  "(" + l->ToString() + ")" + "*" + "(" + r->ToString() + ")";
	}
};

ExpressionPtr Value(int value) {
	return std::make_unique<NumExpr>(value);
}
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
	return std::make_unique<SumExpr>(move(left), move(right));
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
	return std::make_unique<ProductExpr>(move(left), move(right));
}
