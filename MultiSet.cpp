#include "MultiSet.h"

using std::ofstream;
using std::ifstream;

constexpr unsigned BUCKET_SIZE = 8;

static unsigned closestDividableBy8(unsigned n)
{
	while (n % 8 != 0) 
	{
		n++;
	}

	return n;
}

unsigned MultiSet::getN() const
{
	return n;
}

unsigned MultiSet::getK() const
{
	return k;
}

unsigned MultiSet::getBucketsCount() const
{
	return bucketsCount;
}

void MultiSet::free() 
{
	n = 0;
	k = 0;
	bucketsCount = 0;
	delete[] buckets;
	buckets = nullptr;
}

void MultiSet::copyFrom(const MultiSet& other) 
{
	n = other.n;
	k = other.k;
	bucketsCount = other.bucketsCount;
	buckets = new uint8_t[other.bucketsCount];
	
	for (size_t i = 0; i < bucketsCount; i++)
	{
		buckets[i] = other.buckets[i];
	}
}

MultiSet::MultiSet(unsigned n, unsigned k) : n(n), k(k)
{
									
	bucketsCount = (closestDividableBy8((n + 1) * k)) / BUCKET_SIZE;

	buckets = new uint8_t[bucketsCount]{ 0 };
}

MultiSet::MultiSet(const MultiSet& other)
{
	copyFrom(other);
}

MultiSet& MultiSet::operator=(const MultiSet& other)
{
	if (this != &other)
	{
		free();
		copyFrom(other);
	}
	return *this;
}

MultiSet::~MultiSet()
{
	free();
}

unsigned MultiSet::getBucketIndex(unsigned num) const
{
	return (num * k) / BUCKET_SIZE;
}

unsigned MultiSet::getBitIndex(unsigned num) const
{
	return (num * k) % BUCKET_SIZE;
}

void MultiSet::addNumber(unsigned num)
{
	if (num > n) {
		return;
	}

	unsigned bucketIndex = getBucketIndex(num);
	unsigned bitIndex = getBitIndex(num);

	//if the number is in one Bucket

	if ((bitIndex + k) <= BUCKET_SIZE) {
		uint8_t temp = buckets[bucketIndex];

		uint8_t mask = (1<<k) - 1;

		//get num from bucket

		uint8_t numBits = (temp >> (BUCKET_SIZE - bitIndex - k)) & mask;

		//check if number is K times in the bucket
		if (numBits >= (1 << k) - 1)
		{
			return;
		}

		numBits++;

		uint8_t clearMask = (1 << k) - 1;

		clearMask <<= (BUCKET_SIZE - bitIndex - k);

		buckets[bucketIndex] |= clearMask;

		//clears bits of num in the bucket 
		buckets[bucketIndex] ^= clearMask;

		numBits <<= (BUCKET_SIZE - bitIndex - k);

		//adds num in the bucket
		buckets[bucketIndex] |= numBits;
	}
	else 
	{
		uint16_t twoBuckets = (buckets[bucketIndex] << BUCKET_SIZE) | buckets[bucketIndex+1];

		uint16_t mask = (1 << k) - 1;

		uint16_t numBits = (twoBuckets >> (2*BUCKET_SIZE - bitIndex - k)) & mask;

		//check if number is K times in the bucket
		if (numBits >= k)
		{
			return;
		}

		numBits++;

		uint16_t clearMask = (1 << k) - 1;

		clearMask <<= (2 * BUCKET_SIZE - bitIndex - k);

		twoBuckets |= clearMask;

		//clears bits of num in the bucket 
		twoBuckets ^= clearMask;

		numBits <<= (2 * BUCKET_SIZE - bitIndex - k);

		//adds num in the bucket

		twoBuckets |= numBits;

		uint8_t bucketOne = twoBuckets >> BUCKET_SIZE;
		uint8_t bucketTwo = twoBuckets;

		buckets[bucketIndex] = bucketOne;
		buckets[bucketIndex + 1] = bucketTwo;
	}
}

unsigned MultiSet::checkNumberRepetition(unsigned num) const 
{
	unsigned bucketIndex = getBucketIndex(num);
	unsigned bitIndex = getBitIndex(num);

	//if the number is in one buckets

	if ((bitIndex + k) <= BUCKET_SIZE)
	{
		uint8_t temp = buckets[bucketIndex];

		uint8_t mask = (1 << k) - 1;

		return (unsigned)((temp >> (BUCKET_SIZE - bitIndex - k)) & mask);
	}
	//if the number is in two buckets
	else 
	{
		uint16_t twoBuckets = (buckets[bucketIndex] << BUCKET_SIZE) | buckets[bucketIndex + 1];

		uint16_t mask = (1 << k) - 1;

		return(unsigned)(twoBuckets >> (2 * BUCKET_SIZE - bitIndex - k)) & mask;
	}
}

void MultiSet::printNumbers() const
{
	for (size_t i = 0; i <= n; i++)
	{
		for (size_t j = 0; j < checkNumberRepetition(i); j++)
		{
			std::cout << i << " ";
		}
	}
}

void MultiSet::printMemory() const
{
	for (int i = 0; i < bucketsCount; i++) 
	{
		for (int j = 7; j >= 0; j--) 
		{
			std::cout << ((buckets[i] >> j) & 1);
		}
		std::cout << " ";
	}
}

void MultiSet::serialize(ofstream& ofs) const
{
	ofs.write((const char*)&n, sizeof(n));
	ofs.write((const char*)&k, sizeof(k));
	ofs.write((const char*)&bucketsCount, sizeof(bucketsCount));
	ofs.write((const char*)buckets, sizeof(uint8_t)*bucketsCount);
}

void MultiSet::deserialize(ifstream& ifs)
{
	free();

	ifs.read((char*)&n, sizeof(n));
	ifs.read((char*)&k, sizeof(k));
	ifs.read((char*)&bucketsCount, sizeof(bucketsCount));
	buckets = new uint8_t[bucketsCount];
	ifs.read((char*)buckets, sizeof(uint8_t) * bucketsCount);
}

MultiSet IntersectionOfSets(const MultiSet& lhs, const MultiSet& rhs)
{
	MultiSet result(std::min(lhs.n, rhs.n), std::min(lhs.k, rhs.k));

	unsigned minBucketsCount = std::min(lhs.bucketsCount, rhs.bucketsCount);

	for (size_t i = 0; i < minBucketsCount; i++)
	{
		if (lhs.checkNumberRepetition(i) && rhs.checkNumberRepetition(i)) 
		{
			result.addNumber(i);
		}
	}

	return result;
}

MultiSet Difference(const MultiSet& lhs, const MultiSet& rhs)
{
	MultiSet result(lhs.n, lhs.k);

	for (size_t i = 0; i < lhs.n; i++)
	{
		if (rhs.n < i) 
		{
			result.addNumber(i);
		}
		if (rhs.checkNumberRepetition(i) >= lhs.checkNumberRepetition(i)) 
		{
			continue;
		}
		else
		{
			for (size_t j = 0; j < lhs.checkNumberRepetition(i) - rhs.checkNumberRepetition(i); j++)
			{
				result.addNumber(i);
			}
		}
	}

	return result;
}

MultiSet MultiSet::Addition()const 
{
	MultiSet result(n, k);

	for (size_t i = 0; i < bucketsCount; i++)
	{
		result.buckets[i] = ~buckets[i];
	}

	return result;
}
