/*
 * File:   lrucache.hpp
 * Author: Alexander Ponomarev
 *
 * Created on June 20, 2013, 5:09 PM
 */
// clang-format off
#ifndef _LRUCACHE_HPP_INCLUDED_
#define	_LRUCACHE_HPP_INCLUDED_

#include <unordered_map>
#include <list>
#include <cstddef>
#include <stdexcept>
#include <functional>

namespace cache {

template<typename key_t, typename value_t>
class lru_cache {
    static constexpr size_t empty_element = 1;
public:    
    using key_value_pair_t =  typename std::pair<key_t, value_t> ;
    using container_list_t =  typename std::list<key_value_pair_t>;
    using list_iterator_t  =  typename container_list_t::iterator ;    

private:
    using container_map_t = std::unordered_map<key_t, list_iterator_t>;
    using map_iterator_t   =  typename container_map_t::iterator;

public:
    lru_cache(size_t max_size) :
        _max_size(max_size) {        
        _cache_items_map.reserve(_max_size + empty_element);
    }

    void put(const key_t& key, const value_t& value) {
        auto it = _cache_items_map.find(key);
        _cache_items_list.push_front(key_value_pair_t(key, value));
        if (it != _cache_items_map.end()) {
            _cache_items_list.erase(it->second);
            _cache_items_map.erase(it);
        }
        _cache_items_map[key] = _cache_items_list.begin();

        if (_cache_items_map.size() > _max_size) {
            auto last = _cache_items_list.end();
            last--;
            _cache_items_map.erase(last->first);
            _cache_items_list.pop_back();
        }
    }

    void put(key_t&& key, value_t&& value) {
        if (map_iterator_t it = _cache_items_map.find(key); it != std::end(_cache_items_map)) {
            _cache_items_list.erase(it->second);
            _cache_items_map.erase(it);
            this->put(std::forward<key_t>(key), std::forward<value_t>(value));
        } else{
            _cache_items_list.push_front(key_value_pair_t{std::forward<key_t>(key), std::forward<value_t>(value)});
            _cache_items_map[key] = _cache_items_list.begin();

            if (_cache_items_map.size() > _max_size) {
                auto last = _cache_items_list.end();
                last--;
                _cache_items_map.erase(last->first);
                _cache_items_list.pop_back();
            }
        }
    }

    const value_t& get(const key_t& key) {
        auto it = _cache_items_map.find(key);
        if (it == _cache_items_map.end()) {
            throw std::range_error("There is no such key " + std::to_string(key) +  " in cache");
        } else {
            _cache_items_list.splice(_cache_items_list.begin(), _cache_items_list, it->second);
            return (it->second->second);
        }
    }

    void erase(const key_t& key) {        
        if (map_iterator_t it = _cache_items_map.find(key); it == _cache_items_map.end()) {
            throw std::range_error("There is no such key " + std::to_string(key) +  " in cache");
        } else {                        
            _cache_items_list.erase(it->second);
            _cache_items_map.erase(it);
        }
    }

    bool exists(const key_t& key) const {
        return _cache_items_map.find(key) != _cache_items_map.end();
    }

    bool exists(const key_t& key, list_iterator_t& iter) {
        if(auto it = _cache_items_map.find(key);it!=_cache_items_map.end()){
            iter = it->second;
            _cache_items_list.splice(_cache_items_list.begin(),_cache_items_list,it->second);            
            return true;
        }        

        iter = std::end(_cache_items_list);
        return false;
    }

    std::pair<bool,list_iterator_t> get_if_exists(const key_t& key){
        list_iterator_t iter=std::end(_cache_items_list);
        return std::make_pair(exists(key,iter),iter);
    }

    size_t size() const {
        return _cache_items_map.size();
    }

private:
    container_list_t _cache_items_list;
    container_map_t  _cache_items_map;
    size_t _max_size;
};

} // namespace cache

#endif	/* _LRUCACHE_HPP_INCLUDED_ */

