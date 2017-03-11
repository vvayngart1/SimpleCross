#pragma once

#include <memory>
#include <unordered_set>
#include <iterator>

namespace trading {
	template<typename TItem>
	class Pool {
		typedef std::unordered_set<TItem*> TItems;
		typedef std::vector<TItem*> TItemsArray;

	public:
		class Deleter {
			friend class Pool;

		public:
			void operator()(TItem* item) {
				const_cast<Pool&>(_parent).release(item);
			}

		private:
			Deleter(const Pool& parent) : _parent(parent) {
			}

		private:
			const Pool& _parent;
		};

		Deleter getDeleter() const {
			return Deleter(*this);
		}

	public:
		Pool(uint32_t size = 2 << 6) {
			clear();

			TItemsArray items;
			items.resize(size);			
			std::for_each(items.begin(), items.end(), [](TItem*& item) { item = new TItem(); });

			_items.insert(items.begin(), items.end());
			_availableItems.insert(_items.begin(), _items.end());
		}

		~Pool() {
			clear();
		}

		void clear() {
			TItemsArray items;
			std::copy(_items.begin(), _items.end(), std::back_inserter(items));

			_availableItems.clear();
			_items.clear();

			std::for_each(items.begin(), items.end(), [](TItem*& item) { delete item; item = nullptr; });
		}

	public:
		TItem* obtain() {			
			TItem* item = NULL;

			if (_availableItems.empty()) {
				item = new TItem();
				_items.insert(item);
			}
			else {
				typename TItems::iterator iter = _availableItems.begin();
				item = *iter;
				_availableItems.erase(item);
			}

			return item;
		}

		void release(TItem* item) {
			if (NULL == item) {
				//LOG ERRO: "attempting to return NULL to the pool";
				return;
			}

			if (!exists(item)) {
				//LOG ERRO: "attempting to return non-managed object to the pool";
				return;
			}

			if (available(item)) {
				//LOG ERRO: "attempting to return already returned object to the pool";
				return;
			}
			
			_availableItems.insert(item);
		}

		bool exists(TItem* item) const {			
			return (_items.find(item) != _items.end());
		}

		bool available(TItem* item) const {			
			return (_availableItems.find(item) != _availableItems.end());
		}

		size_t capacity() const {			
			return _items.size();
		}

		size_t size() const {			
			return _availableItems.size();
		}
		
	private:
		Pool(const Pool&) = delete;
		Pool & operator=(const Pool&) = delete;

	private:		
		TItems _availableItems;
		TItems _items;
	};
}