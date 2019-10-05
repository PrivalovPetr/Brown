#include "Common.h"

#include <algorithm>

using namespace std;

class Shape : public IShape {
protected:
	Point pos;
	Size size;
	shared_ptr<ITexture> texture;
public:
	virtual ~Shape() = default;

	void SetPosition(Point p) {
		pos = p;
	}
	Point GetPosition() const {
		return pos;
	}

	void SetSize(Size s) {
		size = s;
	}
	Size GetSize() const {
		return size;
	}

	void SetTexture(std::shared_ptr<ITexture> t) {
		texture = t;
	}
	ITexture* GetTexture() const {
		return texture.get();
	}

	virtual void Draw(Image&) const{}
};

class RectangleShape : public Shape {
public:
	RectangleShape() = default;
	
	std::unique_ptr<IShape> Clone() const {
		auto res = std::make_unique<RectangleShape>();
		res->SetPosition(pos);
		res->SetSize(size);
		res->SetTexture(texture);
		return res;
	}

	void Draw(Image& image) const {
		
		for (int i = 0; i < min(size.height, (int)image.size() - pos.y); i++) {
			for (int j = 0; j < min(size.width, (int)image[0].size() - pos.y); j++) {
				if (texture) {
					if (i < texture.get()->GetSize().height && j < texture.get()->GetSize().width) {
						image[pos.y + i][pos.x + j] = texture->GetImage()[i][j];
						continue;
					}
				}
				image[pos.y + i][pos.x + j] = '.';
			}
		}
	}
};

class EllipseShape : public Shape {
public:
	EllipseShape() = default;

	std::unique_ptr<IShape> Clone() const {
		auto res = std::make_unique<EllipseShape>();
		res->SetPosition(pos);
		res->SetSize(size);
		res->SetTexture(texture);
		return res;
	}
	
	void Draw(Image& image) const {
		for (int i = 0; i < min(size.height, (int)image.size() - pos.y); i++) {
			for (int j = 0; j < min(size.width, (int)image[0].size() - pos.y); j++) {
				if (IsPointInEllipse({ j, i }, size)) {
					if (texture) {
						if (i < texture.get()->GetSize().height && j < texture.get()->GetSize().width) {
							image[pos.y + i][pos.x + j] = texture->GetImage()[i][j];
							continue;
						}
					}
					image[pos.y + i][pos.x + j] = '.';
				}
			}
		}
	}
};

unique_ptr<IShape> MakeShape(ShapeType shape_type) {
	if (shape_type == ShapeType::Rectangle) {
		return std::make_unique<RectangleShape>();
	}
	if (shape_type == ShapeType::Ellipse) {
		return std::make_unique<EllipseShape>();
	}
	return nullptr;
}
