#pragma once

namespace RAII {

	template<typename T>
	class Booking {
		T* provider;
		int id;
		bool move = false;
	public:
		Booking(T* p, int i) : provider(p), id(i)
		{}
		Booking(const Booking&) = delete;
		Booking(Booking&& b) {
			this->provider = b.provider;
			this->id = b.id;
			b.move = true;
		}

		Booking& operator=(const Booking&) = delete;
		Booking& operator=(Booking&& b) {
			if (provider->counter >= id) {
				provider->CancelOrComplete(*this);
			}
			provider = b.provider;
			id = b.id;
			b.move = true;
			return *this;
		}
		
		~Booking() {
			if (move)
				return;
			provider->CancelOrComplete(*this);
			
		}
	};
}
