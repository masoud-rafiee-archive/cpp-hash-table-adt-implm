//implementing an unordered map ADT using a hash table(separate chaining or double hashing) as the underlying data structure.We may use std libraries for the linked list(separate chaining) and hash functions.If the hash table becomes overloaded(𝜆 > 0.5) it should allocate more space and rehash all the existing elements.with the BIG FIVE
#include<list>
#include<unordered_map>
#include <iostream>
#include <utility> //for returning the pairs from the functions
#include <algorithm> // std::swap

/* implementing an unordered map ADT using a hash table (separate chaining or double hashing) as the underlying data structure. We may use std libraries for the linked list (separate chaining) and hash functions. If the hash table becomes overloaded (𝜆>0.5) it should allocate more space and rehash all the existing elements.with the BIG FIVE */


template <typename Key, typename T>
class UnMap {
private:
	struct KeyValue {
		Key key;
		T value;
		KeyValue(const Key& k, const T& v) :key(k), value(v) {}
	};
	std::list<KeyValue>* Table;//Hash Table
	int capacity;//current capacity of the hash table
	int size;
	const double load_factor = 0.5;//lamda,load factor threshold

	int hash_function(const Key& key);//hash function to map keys to indices in hash table
	int hash(const Key& key) {
		std::hash<Key> hasher;
		return hasher(key) % capacity;
	}
	void rehash() {
		//cmpute new capacity
		int newcapacity = PrimeNext(capacity * 2);
		//ceate a new table with the new capacity
		std::list<KeyValue>* temp = new std::list<KeyValue>[newcapacity];

		//rehash existing elements
		for (int i = 0; i < capacity; ++i) {
			for (const auto& pair : Table[i]) {
				int index = hash(pair.key); // Compute new hash value
				temp[index].emplace_back(pair); // Insert the hash table pairs into the temp
			}
		}

		//swap the contents of the tables
		std::swap(Table, temp);
		delete[] temp; //delete the temporary table
		capacity = newcapacity; //update the capacity
	}

	bool isPrime(int n) {//for chechikng if a number is prime or not
		if (n <= 1) return false;
		for (int i = 2; i * i <= n; i++) {
			if (n % i == 0) return false;
		}
		return true;
	}
	int PrimeNext(int n) {//getting the prime number
		while (isPrime(n) == false) {
			n++;//going to next n to see if it's prime so we can use it as the capacity 
		}
		return n;
	}

public:
	class Iterator {
	private:
		using TableItr = typename std::list<KeyValue>::iterator;//iterator for the hash table
		TableItr current;//iterator for the current element in the hash table
		int index;//index of the currenct bucket in the hash table
		const UnMap* maptr;//pointer to the unordered map
	public:
		//constructor 
		Iterator(TableItr it, int i, const UnMap* map) :current(it), index(i), maptr(map) {}
		//dereference the operator
		KeyValue& operator*() const { return *current; }
		//arrow operator
		KeyValue* operator->()const { return &(*current); }
		//increment operator (pre increment)
		Iterator& operator++() {
			++current;//increment the current iterator
			while (current == maptr->Table[index].end() && index < maptr->capacity) {//if the current iterator is at the end of the bucket and the index is less than the capacity
				++index;
				if (index < maptr->capacity) {//if the index is less than the capacity
					current = maptr->Table[index].begin();//set the current iterator to the beginning of the bucket
				}
			}return *this;
		}
		//equailty operator
		bool operator==(const Iterator& other) const { return current == other.current && index == other.index; }
		//Inequality op
		bool operator!=(const Iterator& other) const { return !(*this == other); }
		//Constructor 
	};
	UnMap() :capacity(13), size(0) {//starting with an initial capacity of 13
		Table = new std::list <KeyValue>[capacity];//allocate memory for initial capacity
	}
	//Destructor 
	~UnMap() {
		delete[] Table;//freeing memory allocated for the hash table
	}
	//CopycoNstructor
	UnMap(const UnMap& other) :capacity(other.capacity), size(other.size) {
		Table = new std::list<KeyValue>[capacity];
		for (int i = 0; i < capacity; ++i) {
			Table[i] = other.Table[i];
		}
	}
	//MoveCOnstructor
	UnMap(UnMap&& other) noexcept : Table(std::move(other.Table)), capacity(other.capacity), size(other.size) {
		other.capacity = 0;
		other.size = 0;
		other.Table = nullptr;
	}
	//CopyAssignmentOperator
	UnMap& operator=(const UnMap& other) {
		if (this != &other) {//avoid self-assignment
			delete[] Table;
			capacity = other.capacity;
			size = other.size;
			Table = new std::list<KeyValue>[capacity];
			for (int i = 0; i < capacity; i++) {
				Table[i] = other.Table[i];
			}
			return*this;
		}
	}
	//MoveAssignmentOperator 
	UnMap&& operator=(UnMap&& other)noexcept {
		if (this != &other) {
			delete[] Table;
			Table = std::move(other.table);
			capacity = other.capacity;
			size = other.size;
			other.capacity = 0;
			other.size = 0;
			other.Table = nullptr;
		}
		return *this;
	}
	//operator[] function
	Iterator operator[](const Key& k) {
		int index = hash(k);
		for (auto itr = Table[index].begin(); itr != Table[index].end(); ++itr) {
			if (itr->key == k) {
				return Iterator(itr, index, this);//return the iterator
			}
		}
		//if the key is not in the hash table
		Table[index].push_back(KeyValue(k, T()));
		++size;
		return Iterator(std::prev(Table[index].end()), index, this);
	}

	//insert Function 
	std::pair<Iterator, bool> insert(const std::pair<Key, T>& pair) {
		//check load factor to see if we need rehash
		if (static_cast<double>(size) / capacity > load_factor) { rehash(); }
		int index = hash(pair.first);//hash value
		for (auto itr = Table[index].begin(); itr != Table[index].end(); ++itr) {
			//iterating through the bucket
			if (itr->key == pair.first) {//if the key already exists in the hash table
				return std::make_pair(Iterator(itr, index, this), false);//return the iterator and a bool value of false
			}
		}
		//Inserting the new key-value pair
		Table[index].push_back(KeyValue(pair.first, pair.second));
		++size;
		//return an iterator and true indicating successful insert
		return std::make_pair(Iterator(std::prev(Table[index].end()), index, this), true);
	}
	//erase function
	int erase(const Key& k) {
		int index = hash(k);
		//finding the elem to remove
		for (auto itr = Table[index].begin(); itr != Table[index].end(); ++itr) {
			if (itr->key == k) {
				Table[index].erase(itr);
				--size;
				return 1;//successful erase
			}
		}
		return 0;//return 0 if the element is not found
	}
	//clear function
	void clear()noexcept {
		delete[]Table;//reseting the table to default constructor
		capacity = 13;
		size = 0;
		Table = new std::list<KeyValue>[capacity];
	}

	//find function
	Iterator find(const Key& k) {
		int index = hash(k);
		for (auto it = Table[index].begin(); it != Table[index].end(); ++it) {
			if (it->key == k) return Iterator(it, index, this); // Construct an instance of UnMap<Key, T>::Iterator
		}
		return end(); // Return end iterator if not found
	}
	bool contains(const Key& k) {
		int index = hash(k);
		for (const auto& kk : Table[index]) {
			if (kk.key == k) {
				return true;
			}
		}
		return false;
	}
	//empty
	bool empty() const noexcept { return size == 0; }
	//size
	int Size() const noexcept {
		return size;
	}
	//max size
	int max_size() const noexcept { return capacity; }
	//begin function to return the iterator to the beginning of the hash table
	Iterator begin() const {
		int i = 0;
		while (i < capacity && Table[i].empty()) {//find the first non-empty bucket
			++i;
		}
		if (i < capacity) {
			return Iterator(Table[i].begin(), i, this);
		}
		else {
			//if all buckets are empty return end
			return end();
		}
	}
	//end function to return the iterator to the end of the hash table
	Iterator end()const { return Iterator(Table[capacity - 1].end(), capacity, this); }

	
};


int main() {
	// Create an instance of UnMap
	UnMap<int, std::string> MasoudMap;

	// Insert some key-value pairs
	MasoudMap.insert(std::make_pair(1, "One"));
	MasoudMap.insert(std::make_pair(2, "Two"));
	MasoudMap.insert(std::make_pair(3, "Three"));

	// Use operator[] to access elements
	std::cout << "Value for key 2: " << MasoudMap[2]->value << std::endl;

	// Use insert function and check if insertion was successful
	auto result = MasoudMap.insert(std::make_pair(4, "Four"));
	if (result.second) {
		std::cout << "Insertion successful: " << result.first->value << std::endl;
	}
	else {
		std::cout << "Key already exists: " << result.first->value << std::endl;
	}

	// Erase an element and check if it's removed
	int erased = MasoudMap.erase(3);
	if (erased) {
		std::cout << "Element with key 3 erased successfully." << std::endl;
	}
	else {
		std::cout << "Element with key 3 not found." << std::endl;
	}

	// Clear the map and check if it's empty
	MasoudMap.clear();
	if (MasoudMap.empty()) {
		std::cout << "Map cleared successfully." << std::endl;
	}
	else {
		std::cout << "Map is not empty after clearing." << std::endl;
	}

	// Test size and max_size functions
	std::cout << "Size of the map: " << MasoudMap.Size() << std::endl;
	std::cout << "Max size of the map: " << MasoudMap.max_size() << std::endl;

	// Test find and contains functions
	MasoudMap.insert(std::make_pair(5, "Five"));
	auto it = MasoudMap.find(5);
	if (it != MasoudMap.end()) {
		std::cout << "Element with key 5 found: " << it->value << std::endl;
	}
	else {
		std::cout << "Element with key 5 not found." << std::endl;
	}

	if (MasoudMap.contains(6)) {
		std::cout << "Element with key 6 found." << std::endl;
	}
	else {
		std::cout << "Element with key 6 not found." << std::endl;
	}

	// Iterate through the map and print all elements
	std::cout << "Iterating through the map:" << std::endl;
	for (auto it = MasoudMap.begin(); it != MasoudMap.end(); ++it) {
		std::cout << it->key << ": " << it->value << std::endl;
	}

	return 0;

}