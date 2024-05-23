#ifndef BA_GRAPH_ALGORITHMS_OBJECT_COLLECTION_OBJECT_COLLECTION_HPP
#define BA_GRAPH_ALGORITHMS_OBJECT_COLLECTION_OBJECT_COLLECTION_HPP

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ba_graph
{

template <typename CO, typename P>
concept bool CollectibleObject = requires(CO co, const CO &other, const std::string &s, bool do_inflate)
{
	{co.size()} -> int;
	{co.hash()} -> int;

	{co.compute_property()} -> void;
	{co.property_computed()} -> bool;
	{co.property_value()} -> P;

	{co == other} -> bool;

	{co.is_minimised()} -> bool;
	{co.minimise()} -> void;
	{co.inflate()} -> void;

	{co.serialise()} -> std::string;
	{co.deserialise(s, do_inflate)} -> void;
	{new CO(s, do_inflate)};
};

/*
 * Manages a collection of combinatorial objects (typically graphs with some additional properties
 * which take long to compute, for instance, path traversals).
 * The objects are stored both in memory and in a file (optional).
 * 
 * When objects are added, equality is determined by a strategy chosen by the user (see ContainsAlgorithm).
 * 
 * If keepFailedCandidates given in constructor is true, the collection keeps a list of failed candidates
 * (indexed by hashes) and can quickly check a new candidate even without computing its property value.
 * To minimize memory requirements, those failed candidates are kept in a minimized state where only
 * hash computation and equality test need to be meaningful.
 * 
 * If keepOnlySmallest given in constructor is true, then for each property value,
 * only the (first) smallest object is stored.
 */
template <typename T, typename P> requires CollectibleObject<T, P>
class ObjectCollection {
	public: enum ContainsAlgorithm { HASH_PROPERTY_EQUALS, PROPERTY_HASH_EQUALS, HASH_EQUALS };

  protected:	
	ContainsAlgorithm containsAlgorithm;
	std::string fileName;	
	bool useFile;	
	bool keepOnlySmallest;
	int failedCandidatesLimit;

	std::list<std::unique_ptr<T>> objects;
	std::map<P, std::list<T *>> objectsViaProperty; // property -> objects
	std::map<int, std::list<T *>> objectsViaHash; // hash -> objects
	std::map<int, std::list<std::unique_ptr<T>>> failedCandidates;
	int failedCandidatesCount = 0;

	void add_to_memory(std::unique_ptr<T> t) {
		auto tt = t.get();
		objects.push_back(std::move(t));
		objectsViaProperty[tt->property_value()].push_back(tt);
		objectsViaHash[tt->hash()].push_back(tt);
	}

	void add_to_file(const T *t)
	{
		if (!useFile)
			return;
		std::ofstream fout(fileName, std::ios_base::app);
		fout << t->serialise() << std::endl;
	}

	void remove_from_memory(const T *t)
	{
		objects.erase(std::remove_if(objects.begin(), objects.end(),
				[&](auto &x) { return x.get() == t; }), objects.end());
		objectsViaProperty[t->propertyValue()].erase(t);
		objectsViaHash[t->hash()].erase(t);
	}

	void remove_from_file(const T *t)
	{
		if (!useFile)
			return;			
		const std::string tempFile = fileName + ".temp";
		std::ofstream fout(tempFile);
		std::ifstream fin(fileName);
		std::string line;
		while (std::getline(fin, line)) {
			if (line != t->serialise())
			fout << line;
		}
		std::rename(tempFile.c_str(), fileName.c_str());
	}

  public:
	ObjectCollection(const ObjectCollection &) = delete;

	ObjectCollection(const std::string &fileName, ContainsAlgorithm containsAlgorithm,
			bool keepOnlySmallest,
			int failedCandidatesLimit, bool readFileAtInit = true) :
			containsAlgorithm(containsAlgorithm),
			fileName(fileName), useFile(fileName != ""),
			keepOnlySmallest(keepOnlySmallest),
			failedCandidatesLimit(failedCandidatesLimit)
	{
		if (useFile && readFileAtInit) {
			std::ifstream fin(fileName);
			std::string line;
			while (std::getline(fin, line)) {
				add_to_memory(std::move(std::make_unique<T>(line, true)));
			}
		}
	}

	void clear()
	{
		objects.clear();
		objectsViaProperty.clear();
		objectsViaHash.clear();
		failedCandidates.clear();

		if (useFile)
			::remove(fileName.c_str());
	}

	void write_to_file(const std::string &file, bool append=false)
	{
		std::ofstream fout;
		auto flags = (append) ? (std::ios::app | std::ios::out) : std::ios::out;
		fout.open(file, flags);
		for (auto &t : objects)
			fout << t->serialise() << std::endl;
	}

	bool contains_hash_property_equals(T *t)
	{
		auto &candidates = objectsViaHash[t->hash()];
		if (!t->property_computed())
			t->compute_property();
		for (auto x : candidates) {
			if (x->property_value() == t->property_value() && *x == *t)
				return true;						
		}
		return false;
	}

	bool contains_property_hash_equals(T *t)
	{
		if (!t->property_computed())
			t->compute_property();
		auto &candidates = objectsViaProperty[t->property_value()];
		for (auto x : candidates) {
			if (x->hash() == t->hash() && *x == *t)
				return true;						
		}
		return false;
	}

	bool contains_hash_equals(const T *t)
	{
		auto &c = objectsViaHash[t->hash()];
		auto cmp = [=](const T *x) { return *x == *t; };
		return std::find_if(c.begin(), c.end(), cmp) != c.end();
	}

	bool contains(T *t) {
		switch (containsAlgorithm) {
			case HASH_PROPERTY_EQUALS:
				return contains_hash_property_equals(t);
			case PROPERTY_HASH_EQUALS:
				return contains_property_hash_equals(t);
			case HASH_EQUALS:
				return contains_hash_equals(t);
			default:
				throw std::logic_error("unknown containsAlgorithm ???");
		}
	}

	bool add(std::unique_ptr<T> t, bool checkEquality) {
		if (is_failed_candidate(t.get()))
			return false;
		// we want to add t either to objects or to failedCandidates
		bool addToObjects = !checkEquality || !contains(t.get());
		if (!addToObjects) {
			add_failed_candidate(std::move(t));
			return false;
		}

		if (!t->property_computed())
			t->compute_property();
		auto &existing = objectsViaProperty[t->property_value()];
		if (keepOnlySmallest && existing.size() > 0) {
			if (t->size() >= existing.front()->size())
				addToObjects = false;
			else
				remove_from_file(existing.front());
		}

		if (addToObjects) {
			add_to_file(t.get());
			add_to_memory(std::move(t));
		} else {
			add_failed_candidate(std::move(t));
		}
		return addToObjects;
	}

	void add_failed_candidate(std::unique_ptr<T> t)
	{
		if (failedCandidatesCount >= failedCandidatesLimit)
			return;
		t->minimise();
		failedCandidates[t->hash()].push_back(std::move(t));
		failedCandidatesCount++;
	}

	bool is_failed_candidate(const T *t)
	{
		auto &c = failedCandidates[t->hash()];
		auto cmp = [=](const std::unique_ptr<T> &x) { return *x.get() == *t; };
		return std::find_if(c.begin(), c.end(), cmp) != c.end();
	}

	void remove(T *object)
	{
		remove_from_memory(object);
		remove_from_file(object);
	}

	int size() const { return objects.size(); }

	int count(int size) const
	{
		int count = 0;
		for (auto t : objects) {
			if (t->size() == size)
				count++;
		}
		return count;
	}

	int count(const P &propertyValue) const
	{
		return objectsViaProperty[propertyValue].size();
	}

	int count(int size, const P &propertyValue) const
	{
		int count = 0;
		for (auto t : objectsViaProperty[propertyValue]) {
			if (t->size() == size)
				count++;
		}
		return count;
	}

	std::list<const T *> get_all() const
	{
		std::list<const T *> result;
		for (const auto &t : objects)
			result.push_back(t.get());
		return result;
	}

	std::list<const T *> get_all_by_size(int size) const
	{
		std::list<const T *> result;
		for (const auto &t : objects) {
			if (t->size() == size)
				result.push_back(t.get());
		}
		return result;
	}

	std::list<const T *> get_all_by_property(const P &propertyValue) const
	{
		std::list<const T *> result = objectsViaProperty[propertyValue];
		std::sort(result.begin(), result.end(), [](const T *t1, const T *t2) { return t1->size() < t2->size(); } );
		return result;
	}

	std::list<const T *> get_all_by_size_and_property(int size, const P &propertyValue) const
	{
		std::list<const T *> result;
		for (auto t : objectsViaProperty[propertyValue]) {
			if (t->size() == size)
				result.push_back(t);
		}
		return result;
	}

	std::map<P, std::list<T *>> get_all_via_property()
	{
		return objectsViaProperty;
	}

	std::map<int, std::list<T *>> get_all_via_hash()
	{
		return objectsViaHash;
	}
};

} // namespace ba_graph

#endif
