/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "LibC.h"

static inline void* alloc_proxy(const struct Allocator*, size_t size) {
   return malloc(size);
}

static inline void free_proxy(const struct Allocator*, void* mem) {
   free(mem);
}

const struct Allocator DEFAULT_ALLOCATOR = { &alloc_proxy, &free_proxy };
static const struct Allocator* current_allocator = &DEFAULT_ALLOCATOR;

const struct Allocator* set_default_allocator(const struct Allocator *a) {
   const struct Allocator *b = current_allocator;
   current_allocator = a;

   return b;
}

void* operator new(size_t lSize) {
   return operator new(lSize, current_allocator);
}

void* operator new[](size_t lSize) {
   return operator new[](lSize, current_allocator);
}

void* operator new(size_t lSize, const struct Allocator *a) {
   void** mem = (void**)a->alloc(a, lSize + sizeof(IPTR));
   *mem++ = const_cast<Allocator*>(a);
   return mem;
}

void* operator new[](size_t lSize, const struct Allocator *a) {
   void** mem = (void**)a->alloc(a, lSize + sizeof(IPTR));
   *mem++ = const_cast<Allocator*>(a);
   return mem;
}

void* operator new(size_t lSize, const Allocator* a, std::align_val_t align) noexcept {
    std::size_t alignment = static_cast<std::size_t>(align);
    std::size_t extra = alignment - 1 + sizeof(void*);
    void* raw = a->alloc(a, lSize + extra);
    uintptr_t addr = reinterpret_cast<uintptr_t>(raw) + sizeof(void*);
    uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
    void** mem = reinterpret_cast<void**>(aligned);
    mem[-1] = raw;  // store original pointer for deallocation
    return mem;
}

void* operator new(size_t lSize, std::align_val_t align) {
    return operator new(lSize, current_allocator, align);
}

void* operator new[](size_t lSize, const Allocator* a, std::align_val_t align) {
    return operator new(lSize, a, align);
}

void* operator new[](size_t lSize, std::align_val_t align) {
    return operator new[](lSize, current_allocator, align);
}
