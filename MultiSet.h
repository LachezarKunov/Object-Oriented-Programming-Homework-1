#pragma once
#include <iostream>
#include <fstream>

class MultiSet
{
private:
	unsigned n = 0;
	short k = 0;
	uint8_t* buckets = nullptr;
	unsigned bucketsCount = 0;

	void free();
	void copyFrom(const MultiSet& other);

	unsigned getBucketIndex(unsigned num) const;
	unsigned getBitIndex(unsigned num) const;

public:
	MultiSet() = default;
	MultiSet(unsigned n, unsigned k);
	MultiSet(const MultiSet& other);
	MultiSet& operator=(const MultiSet& other);
	~MultiSet();

	unsigned getN() const;
	unsigned getK() const;
	unsigned getBucketsCount() const;

	void addNumber(unsigned num);
	unsigned checkNumberRepetition(unsigned num) const;
	void printNumbers()const;
	void printMemory()const;

	void serialize(std::ofstream& ofs) const;
	void deserialize(std::ifstream& ifs);

	friend MultiSet IntersectionOfSets(const MultiSet& lhs, const MultiSet& rhs);
	friend MultiSet Difference(const MultiSet& lhs, const MultiSet& rhs);

	MultiSet Addition() const;
};
