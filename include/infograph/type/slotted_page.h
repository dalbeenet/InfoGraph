﻿#ifndef _INFORGRAPH_TYPE_SLOTTED_PAGE_H_
#define _INFORGRAPH_TYPE_SLOTTED_PAGE_H_
#include <cstring>
#include <cstdint>
#include <type_traits>
#include <memory>
#include <infograph/mpl.h>

/* ---------------------------------------------------------------
**
** InfoGraph - InfoLab Graph Library
**
** slotted_page.h
** Generic C++ implementation of the "Slotted Page" which is a
** data structure for graph-data processing
**
** Author: Seyeon Oh (vee@dgist.ac.kr)
** ------------------------------------------------------------ */

/* ---------------------------------------------------------------
**
** Slotted page represntation (Page size: user defined)
** +-------------------------------------------------------------+
** |                                                             |
** |                                                             |
** |                      Data Section                           |
** |               size = page size - footer size       +--------+
** |                                                    | footer |
** +----------------------------------------------------+--------+
**
** Detailed slotted page representation
** +-------------------------------------------------------------+
** | S0 adj-list size | S0 adj-elem #0 | S0 adj-elem #1 | S0 adj-|
** +-------------------------------------------------------------+
** |elem #2 | S0 adj-elem #3 | ...                               |
** +-------------------------------------------------------------+
** |                                                             |
** +-------------------------------------------------------------+
** |                ... | S0 adj-elem #N | S1 adj-list size | S1 |
** +-------------------------------------------------------------+
** | adj-elem #0 | S2 adj-elem #1 | S1 adj-elem #2 | S1 adj-elem |
** +-------------------------------------------------------------+
** | #3 | ...                                                    |
** +-------------------------------------------------------------+
** |                                                             |
** +-------------------------------------------------------------+
** |                                    ...| S1 adj-elem #M |    |
** +-------------------------------------------------------------+
** |                                                             |
** +-------------------------------------------------------------+
** |                 | slot #1 (S1) | slot #0 (S0) | page footer |
** +-------------------------------------------------------------+
**
** Adjacency list-size: user defined type
** +--------------------------------------------+
** | Slot # adjacency list size (user-def type) |
** +--------------------------------------------+
**
** Adjacency list-element (Edge) representation
** +-----------------------------------------------------------------------------------------+
** | adj_page_id (user-def type) | adj_offset (user-def type) | edge-payload (user-def type) |
** +-----------------------------------------------------------------------------------------+
**
** Slot (Vertex) representation
** +--------------------------------------------------------------------------------------------+
** | vertex_id (user-def type) | vertex-payload (user-def type) | record_offset (user-def type) |
** +--------------------------------------------------------------------------------------------+
**
** Page footer representation: default 16 bytes
** @offset_t: user-def type, default = uint32_t
** +--------------------------------------------------------------------------+
** | reserved (4byte) | flags (uint32_t) | front (offset_t) | rear (offset_t) |
** +--------------------------------------------------------------------------+
**
** ------------------------------------------------------------ */

namespace igraph {

constexpr size_t SIZE_1KB = 1024u;
constexpr size_t SIZE_1MB = SIZE_1KB * 1024u;
constexpr size_t SIZE_1GB = SIZE_1MB * 1024u;

#define SLOTTED_PAGE_TEMPLATE \
template <\
    typename __vertex_id_t,\
    typename __page_id_t,\
    typename __record_offset_t,\
    typename __slot_offset_t,\
    typename __adj_list_size_t,\
    size_t   __page_size,\
    typename __edge_payload_t,\
    typename __vertex_payload_t,\
    typename __offset_t\
>

#define SLOTTED_PAGE_TEMPLATE_ARGS \
    __vertex_id_t,\
    __page_id_t,\
    __record_offset_t,\
    __slot_offset_t,\
    __adj_list_size_t,\
    __page_size,\
    __edge_payload_t,\
    __vertex_payload_t,\
    __offset_t

namespace slotted_page_flag {
constexpr uint32_t _BASE = 0x0001;
constexpr uint32_t SP = _BASE;
constexpr uint32_t LP_HEAD = _BASE << 1;
constexpr uint32_t LP_EXTENDED = _BASE << 2;
} // !namespace slotted_page_flag

namespace _slotted_page {

#pragma pack (push, 1)

template <typename __page_id_t,
    typename __slot_offset_t,
    typename __edge_payload_t>
    struct adj_list_element
{
    __page_id_t      page_id;
    __slot_offset_t  slot_offset;
    __edge_payload_t payload;
};

template <
    typename __page_id_t,
    typename __slot_offset_t>
    struct adj_list_element<__page_id_t, __slot_offset_t, void>
{
    __page_id_t      page_id;
    __slot_offset_t  slot_offset;
    /*adj_list_elem_base() = default;
    adj_list_elem_base(const adj_list_elem_base& other):
    page_id { other.page_id },
    slot_offset { other.slot_offset },
    payload { other.payload } { }
    adj_list_elem_base(const page_id_t& page_id_, const slot_offset_t& slot_offset_, const edge_payload_t payload_):
    page_id { page_id_ },
    slot_offset { slot_offset_ },
    payload { payload_ } { }
    ~adj_list_elem_base() = default;*/
};

template <
    typename __vertex_id_t,
    typename __record_offset_t,
    typename __vertex_payload_t>
    struct slot
{
    __vertex_id_t      vertex_id;
    __record_offset_t  record_offset;
    __vertex_payload_t vertex_payload_t;
};

template <
    typename __vertex_id_t,
    typename __record_offset_t>
    struct slot<__vertex_id_t, __record_offset_t, void>
{
    __vertex_id_t     vertex_id;
    __record_offset_t record_offset;
};

using page_flag_t = uint32_t;

template <typename offset_t>
struct footer
{
    uint32_t    reserved;
    page_flag_t flags;
    offset_t    front;
    offset_t    rear;
};

#pragma pack (pop)

} // !namespace _slotted_page

#define SLOTTED_PAGE_TEMPLATE_TYPEDEFS \
    using vertex_id_t = __vertex_id_t;\
    using page_id_t = __page_id_t;\
    using record_offset_t = __record_offset_t;\
    using slot_offset_t = __slot_offset_t;\
    using adj_list_size_t = __adj_list_size_t;\
    using edge_payload_t = __edge_payload_t;\
    using vertex_payload_t = __vertex_payload_t;\
    using offset_t = __offset_t;\
    using page_flag_t = _slotted_page::page_flag_t;\
    using adj_list_elem_t = _slotted_page::adj_list_element<page_id_t, slot_offset_t, edge_payload_t>;\
    using slot_t = _slotted_page::slot<vertex_id_t, record_offset_t, vertex_payload_t>;\
    using footer_t = _slotted_page::footer<offset_t>;\
    using ___size_t = target_arch_size_t

#define ALIAS_SLOTTED_PAGE_TEMPLATE_TYPEDEFS(PAGE_T) \
    using vertex_id_t = typename PAGE_T::vertex_id_t;\
    using page_id_t = typename PAGE_T::page_id_t;\
    using record_offset_t = typename PAGE_T::record_offset_t;\
    using slot_offset_t = typename PAGE_T::slot_offset_t;\
    using adj_list_size_t = typename PAGE_T::adj_list_size_t;\
    using edge_payload_t = typename PAGE_T::edge_payload_t;\
    using vertex_payload_t = typename PAGE_T::vertex_payload_t;\
    using offset_t = typename PAGE_T::offset_t;\
    using page_flag_t = typename PAGE_T::page_flag_t;\
    using adj_list_elem_t = typename PAGE_T::adj_list_elem_t;\
    using slot_t = typename PAGE_T::slot_t;\
    using footer_t = typename PAGE_T::footer_t;\
    using ___size_t = typename PAGE_T::___size_t

#define SLOTTED_PAGE_TEMPLATE_CONSTDEFS \
    static constexpr ___size_t PageSize = __page_size;\
    static constexpr ___size_t EdgePayloadSize = mpl::_sizeof<edge_payload_t>::value;\
    static constexpr ___size_t VertexPayloadSize = mpl::_sizeof<vertex_payload_t>::value;\
    static constexpr ___size_t DataSectionSize = PageSize - sizeof(footer_t);\
    static constexpr ___size_t MaximumEdgesInHeadPage = (DataSectionSize - sizeof(slot_t) - sizeof(adj_list_size_t)) / sizeof(adj_list_elem_t);\
    static constexpr ___size_t MaximumEdgesInExtPage = (DataSectionSize - sizeof(slot_t)) / sizeof(adj_list_elem_t);\
    static constexpr ___size_t SlotSize = sizeof(slot_t)

#define ALIAS_SLOTTED_PAGE_TEMPLATE_CONSTDEFS(PAGE_T) \
    static constexpr ___size_t PageSize = PAGE_T::PageSize;\
    static constexpr ___size_t EdgePayloadSize = PAGE_T::EdgePayloadSize;\
    static constexpr ___size_t VertexPayloadSize = PAGE_T::VertexPayloadSize;\
    static constexpr ___size_t DataSectionSize = PAGE_T::DataSectionSize;\
    static constexpr ___size_t MaximumEdgesInHeadPage = PAGE_T::MaximumEdgesInHeadPage;\
    static constexpr ___size_t MaximumEdgesInExtPage = PAGE_T::MaximumEdgesInExtPage;\
    static constexpr ___size_t SlotSize = PAGE_T::SlotSize

#pragma pack (push, 1)
template <
    typename __vertex_id_t,
    typename __page_id_t,
    typename __record_offset_t,
    typename __slot_offset_t,
    typename __adj_list_size_t,
    size_t   __page_size,
    typename __edge_payload_t = void,
    typename __vertex_payload_t = void,
    typename __offset_t = uint32_t
>
class slotted_page
{
    /* Constratint 1. The edge-payload type must be a void type or a Plain old data (POD) type */
    static_assert((std::is_void<__edge_payload_t>::value || std::is_pod<__edge_payload_t>::value),
                  "Generic Slotted Page: Constraint 1. The edge-payload type must be a Plain old data (POD) type");
    /* Constratint 2. The vertex-payload type must be a void type or a Plain old data (POD) type */
    static_assert((std::is_void<__vertex_payload_t>::value || std::is_pod<__vertex_payload_t>::value),
                  "Generic Slotted Page: Constraint 2. The vertex-payload type must be a Plain old data (POD) type");

    /* Typedefs and Constant value definitions */
public:
    using type = slotted_page<SLOTTED_PAGE_TEMPLATE_ARGS>;
    using shared_ptr = std::shared_ptr<type>;
    using unique_ptr = std::unique_ptr<type>;
    using weak_ptr = std::weak_ptr<type>;
    SLOTTED_PAGE_TEMPLATE_TYPEDEFS;
    SLOTTED_PAGE_TEMPLATE_CONSTDEFS;

    /* Member functions */
        // Constructors & Destructor
    slotted_page() = default;
    explicit slotted_page(page_flag_t flag);
    slotted_page(const type& other);
    ~slotted_page() = default;

    // Operators
    inline type& operator=(const type& other);
    inline uint8_t& operator[](offset_t offset)
    {
        return data_section[offset];
    }
    inline bool operator==(const type& other)
    {
        return memcmp(this, &other, sizeof(PageSize)) == 0;
    }

    // Utilites
    inline offset_t number_of_slots() const
    {
        return static_cast<offset_t>((DataSectionSize - this->footer.rear) / sizeof(slot_t));
    }
    inline slot_t& slot(const offset_t offset) 
    {
        return *reinterpret_cast<slot_t*>(&this->data_section[DataSectionSize - (sizeof(slot_t) * (offset + 1))]);
    }
    inline adj_list_size_t& list_size(const slot_t& slot) 
    {
        return *reinterpret_cast<adj_list_size_t*>(&data_section[slot.record_offset]);
    }
    inline adj_list_size_t& list_size(const offset_t slot_offset) 
    {
        return this->list_size(slot(slot_offset));
    }
    inline adj_list_elem_t* list(const slot_t& slot) 
    {
        return reinterpret_cast<adj_list_elem_t*>(&data_section[slot.record_offset + sizeof(adj_list_size_t)]);
    }
    inline adj_list_elem_t* list(const offset_t slot_offset) 
    {
        return reinterpret_cast<adj_list_elem_t*>(&data_section[slot(slot_offset).record_offset + sizeof(adj_list_size_t)]);
    }
    inline adj_list_elem_t* list_ext(const slot_t& slot)
    {
        return reinterpret_cast<adj_list_elem_t*>(&data_section[slot.record_offset]);
    }
    inline adj_list_elem_t* list_ext(const offset_t slot_offset)
    {
        return reinterpret_cast<adj_list_elem_t*>(&data_section[slot(slot_offset).record_offset]);
    }
    inline page_flag_t& flags()
    {
        return footer.flags;
    }

    inline bool is_lp() const
    {
        return 0 != (footer.flags & (slotted_page_flag::LP_HEAD | slotted_page_flag::LP_EXTENDED));
    }
    inline bool is_lp_head() const
    {
        return 0 != (footer.flags & slotted_page_flag::LP_HEAD);
    }
    inline bool is_lp_extended() const
    {
        return 0 != (footer.flags & slotted_page_flag::LP_EXTENDED);
    }
    inline bool is_sp() const
    {
        return 0 != (footer.flags & slotted_page_flag::SP);
    }
    inline bool is_empty() const
    {
        return (footer.front == 0 && footer.rear == DataSectionSize);
    }

    /* Member variables */
public:
    uint8_t  data_section[DataSectionSize];
    footer_t footer{ 0, 0, 0, DataSectionSize };
};

#define SLOTTED_PAGE slotted_page<SLOTTED_PAGE_TEMPLATE_ARGS>

SLOTTED_PAGE_TEMPLATE SLOTTED_PAGE::slotted_page(const type& other)
{
    memmove(this, &other, PageSize);
}

SLOTTED_PAGE_TEMPLATE SLOTTED_PAGE::slotted_page(page_flag_t flags)
{
    footer.flags = flags;
}

SLOTTED_PAGE_TEMPLATE
inline typename SLOTTED_PAGE::type& SLOTTED_PAGE::operator=(const type& other)
{
    memmove(this, &other, PageSize);
    return *this;
}
#pragma pack (pop)

#pragma pack (push, 1)
template <
    typename __vertex_id_t,
    typename __page_id_t,
    typename __record_offset_t,
    typename __slot_offset_t,
    typename __adj_list_size_t,
    size_t   __page_size,
    typename __edge_payload_t = void,
    typename __vertex_payload_t = void,
    typename __offset_t = uint32_t
>
class slotted_page_builder: public slotted_page<SLOTTED_PAGE_TEMPLATE_ARGS>
{
public:
    using page_t = slotted_page<SLOTTED_PAGE_TEMPLATE_ARGS>;
    using type = slotted_page_builder<SLOTTED_PAGE_TEMPLATE_ARGS>;
    using shared_ptr = std::shared_ptr<type>;
    using unique_ptr = std::unique_ptr<type>;
    using weak_ptr = std::weak_ptr<type>;
    SLOTTED_PAGE_TEMPLATE_TYPEDEFS;
    SLOTTED_PAGE_TEMPLATE_CONSTDEFS;

    // Constructors & Destructor
    slotted_page_builder() = default;
    explicit slotted_page_builder(page_flag_t flag);
    slotted_page_builder(const type& other);
    ~slotted_page_builder() = default;

    // Operators
    inline type& operator=(const type& other);
    inline uint8_t& operator[](offset_t offset)
    {
        return this->data_section[offset];
    }
    inline bool operator==(const type& other)
    {
        return memcmp(this, &other, sizeof(PageSize)) == 0;
    }

    /// Scan: Scan the free space of a page and returns a couple of information as follows
    // (1) Whether this page can store a new slot. <Boolean>
    // (2) An available adjacency list-size of the new slot. (If (1) is false, the value is 0.) <page_cnt_t:size-type>
    std::pair<bool/* (1) */, ___size_t /* (2) */> scan() const;
    /// Scan for extended page
    std::pair<bool/* (1) */, ___size_t /* (2) */> scan_ext() const;

    /// Add slot: Add a new slot into a page, returns an offset of new slot
    // Enabled if vertex_payload_t is void type.
    template <typename PayloadTy = vertex_payload_t>
    typename std::enable_if<std::is_void<PayloadTy>::value, offset_t>::type add_slot(vertex_id_t vertex_id);
    // Enabled if vertex_payload_t is non-void type.
    template <typename PayloadTy = vertex_payload_t>
    offset_t add_slot(vertex_id_t vertex_id, typename std::enable_if<!std::is_void<PayloadTy>::value, vertex_payload_t>::type payload);

    /// Add slot for an extened page
    // Enabled if vertex_payload_t is void type.
    template <typename PayloadTy = vertex_payload_t>
    typename std::enable_if<std::is_void<PayloadTy>::value, offset_t>::type add_slot_ext(vertex_id_t vertex_id);
    // Enabled if vertex_payload_t is non-void type.
    template <typename PayloadTy = vertex_payload_t>
    offset_t add_slot_ext(vertex_id_t vertex_id, typename std::enable_if<!std::is_void<PayloadTy>::value, vertex_payload_t>::type payload);

    /// Add dummy slot
    offset_t add_dummy_slot();
    offset_t add_dummy_slot_ext();

    /// Add list: Add a adjacency list to slot[offset]
    /// Add list for a Small Page (SP)
    void add_list_sp(offset_t slot_offset, adj_list_elem_t* elem_arr, ___size_t list_size);
    /// Add list for a head of Large Pages (LP-head)
    void add_list_lp_head(___size_t list_size, adj_list_elem_t* elem_arr, ___size_t num_elems_in_page);
    /// Add list for extended part of Large pages (LP-ext)
    void add_list_lp_ext(adj_list_elem_t* elem_arr, ___size_t num_elems_in_page);

    /// Add dummy list
    void add_dummy_list_sp(offset_t slot_offset, ___size_t list_size);
    void add_dummy_list_lp_head(___size_t list_size, ___size_t num_elems_in_page);
    void add_dummy_list_lp_ext(___size_t num_elems_in_page);

    /// Utilites
    void clear();
};

#define SLOTTED_PAGE_BUILDER slotted_page_builder<SLOTTED_PAGE_TEMPLATE_ARGS>

SLOTTED_PAGE_TEMPLATE
SLOTTED_PAGE_BUILDER::slotted_page_builder(const type& other)
{
    memmove(this, &other, PageSize);
}

SLOTTED_PAGE_TEMPLATE
SLOTTED_PAGE_BUILDER::slotted_page_builder(page_flag_t flags)
{
    this->footer.flags = flags;
}

SLOTTED_PAGE_TEMPLATE
inline typename SLOTTED_PAGE_BUILDER::type& SLOTTED_PAGE_BUILDER::operator=(const type& other)
{
    memmove(this, &other, PageSize);
    return *this;
}

SLOTTED_PAGE_TEMPLATE
std::pair<bool/* (1) */, typename SLOTTED_PAGE_BUILDER::___size_t /* (2) */> SLOTTED_PAGE_BUILDER::scan() const
{
    auto free_space = this->footer.rear - this->footer.front;
    if (free_space < (sizeof(slot_t) + sizeof(adj_list_size_t)))
        return std::make_pair(false, 0); // The page does not have enough space to store new slot.
    free_space -= (sizeof(slot_t) + sizeof(adj_list_size_t));
    return std::make_pair(true, static_cast<___size_t>(free_space / sizeof(adj_list_elem_t)));
}

SLOTTED_PAGE_TEMPLATE
std::pair<bool/* (1) */, typename SLOTTED_PAGE_BUILDER::___size_t /* (2) */> SLOTTED_PAGE_BUILDER::scan_ext() const
{
    auto free_space = this->footer.rear - this->footer.front;
    if (free_space < sizeof(slot_t)) //! Extended page does not need to space to store adjacency list size.
        return std::make_pair(false, 0); // The page does not have enough space to store new slot.
    free_space -= (sizeof(slot_t));
    return std::make_pair(true, static_cast<___size_t>(free_space / sizeof(adj_list_elem_t)));
}

SLOTTED_PAGE_TEMPLATE
template <typename PayloadTy>
typename std::enable_if<std::is_void<PayloadTy>::value, __offset_t>::type SLOTTED_PAGE_BUILDER::add_slot(vertex_id_t vertex_id)
{
    this->footer.rear -= sizeof(slot_t);
    slot_t& slot = reinterpret_cast<slot_t&>(this->data_section[this->footer.rear]);
    slot.record_offset = static_cast<record_offset_t>(this->footer.front);
    slot.vertex_id = vertex_id;
    this->footer.front += sizeof(adj_list_size_t);
    return this->number_of_slots() - 1;
}

SLOTTED_PAGE_TEMPLATE
template <typename PayloadTy>
typename SLOTTED_PAGE_BUILDER::offset_t SLOTTED_PAGE_BUILDER::add_slot(vertex_id_t vertex_id, typename std::enable_if<!std::is_void<PayloadTy>::value, vertex_payload_t>::type payload)
{
    this->footer.rear -= sizeof(slot_t);
    slot_t& slot = reinterpret_cast<slot_t&>(this->data_section[this->footer.rear]);
    slot.vertex_id = vertex_id;
    slot.record_offset = static_cast<record_offset_t>(this->footer.front);
    slot.vertex_payload_t = payload;
    this->footer.front += sizeof(adj_list_size_t);
    return this->number_of_slots() - 1;
}

SLOTTED_PAGE_TEMPLATE
template <typename PayloadTy>
typename std::enable_if<std::is_void<PayloadTy>::value, __offset_t>::type SLOTTED_PAGE_BUILDER::add_slot_ext(vertex_id_t vertex_id)
{
    this->footer.rear -= sizeof(slot_t);
    slot_t& slot = reinterpret_cast<slot_t&>(this->data_section[this->footer.rear]);
    slot.vertex_id = vertex_id;
    slot.record_offset = static_cast<record_offset_t>(this->footer.front);
    return this->number_of_slots() - 1;
}

SLOTTED_PAGE_TEMPLATE
template <typename PayloadTy>
typename SLOTTED_PAGE_BUILDER::offset_t SLOTTED_PAGE_BUILDER::add_slot_ext(vertex_id_t vertex_id, typename std::enable_if<!std::is_void<PayloadTy>::value, vertex_payload_t>::type payload)
{
    this->footer.rear -= sizeof(slot_t);
    slot_t& slot = reinterpret_cast<slot_t&>(this->data_section[this->footer.rear]);
    slot.vertex_id = vertex_id;
    slot.record_offset = static_cast<record_offset_t>(this->footer.front);
    slot.vertex_payload_t = payload;
    return this->number_of_slots() - 1;
}

SLOTTED_PAGE_TEMPLATE
typename SLOTTED_PAGE_BUILDER::offset_t SLOTTED_PAGE_BUILDER::add_dummy_slot()
{
    this->footer.rear -= sizeof(slot_t);
    this->footer.front += sizeof(adj_list_size_t);
    return this->number_of_slots() - 1;
}

SLOTTED_PAGE_TEMPLATE
typename SLOTTED_PAGE_BUILDER::offset_t SLOTTED_PAGE_BUILDER::add_dummy_slot_ext()
{
    this->footer.rear -= sizeof(slot_t);
    return this->number_of_slots() - 1;
}

SLOTTED_PAGE_TEMPLATE
void SLOTTED_PAGE_BUILDER::add_list_sp(const offset_t slot_offset, adj_list_elem_t* elem_arr, ___size_t list_size)
{
    slot_t& slot = this->slot(slot_offset);
    this->list_size(slot) = static_cast<adj_list_size_t>(list_size);
    memmove(this->list(slot), elem_arr, sizeof(adj_list_elem_t) * list_size);
    this->footer.front += static_cast<decltype(this->footer.front)>(sizeof(adj_list_elem_t) * list_size);
}

SLOTTED_PAGE_TEMPLATE
void SLOTTED_PAGE_BUILDER::add_list_lp_head(___size_t list_size, adj_list_elem_t* elem_arr, ___size_t num_elems_in_page)
{
    slot_t& slot = this->slot(0);
    this->list_size(slot) = static_cast<adj_list_size_t>(list_size);
    memmove(this->list(slot), elem_arr, sizeof(adj_list_elem_t) * num_elems_in_page);
    this->footer.front += static_cast<decltype(this->footer.front)>(sizeof(adj_list_elem_t) * num_elems_in_page);
}

SLOTTED_PAGE_TEMPLATE
void SLOTTED_PAGE_BUILDER::add_list_lp_ext(adj_list_elem_t* elem_arr, ___size_t num_elems_in_page)
{
    slot_t& slot = this->slot(0);
    memmove(this->list_ext(slot), elem_arr, sizeof(adj_list_elem_t) * num_elems_in_page);
    this->footer.front += static_cast<decltype(this->footer.front)>(sizeof(adj_list_elem_t) * num_elems_in_page);
}

SLOTTED_PAGE_TEMPLATE
void SLOTTED_PAGE_BUILDER::add_dummy_list_sp(const offset_t slot_offset, ___size_t list_size)
{
    slot_t& slot = this->slot(slot_offset);
    this->list_size(slot.record_offset) = static_cast<adj_list_size_t>(list_size);
    this->footer.front += static_cast<offset_t>(sizeof(adj_list_elem_t) * list_size);
}

SLOTTED_PAGE_TEMPLATE
void SLOTTED_PAGE_BUILDER::add_dummy_list_lp_head(___size_t list_size, ___size_t num_elems_in_page)
{
    slot_t& slot = this->slot(0);
    this->list_size(slot.record_offset) = list_size;
    this->footer.front += sizeof(adj_list_elem_t) * num_elems_in_page;
}

SLOTTED_PAGE_TEMPLATE
void SLOTTED_PAGE_BUILDER::add_dummy_list_lp_ext(___size_t num_elems_in_page)
{
    this->footer.front += sizeof(adj_list_elem_t) * num_elems_in_page;
}

SLOTTED_PAGE_TEMPLATE
void SLOTTED_PAGE_BUILDER::clear()
{
    memset(this->data_section, 0, DataSectionSize);
    this->footer.front = 0;
    this->footer.rear = DataSectionSize;
}

#undef SLOTTED_PAGE_TEMPLATE_TYPEDEFS
#undef SLOTTED_PAGE_TEMPLATE_CONSTDEFS
#undef SLOTTED_PAGE_BUILDER
#undef SLOTTED_PAGE
#undef SLOTTED_PAGE_TEMPLATE_ARGS
#undef SLOTTED_PAGE_TEMPLATE

template <typename __builder_t, typename __rid_table_t>
typename __builder_t::page_id_t vid_to_pid(typename __builder_t::vertex_id_t vid, __rid_table_t& table)
{
    for (typename __builder_t::___size_t i = 0; i < table.size(); ++i)
    {
        const auto& tuple = table[i];
        if (tuple.start_vid == vid)
            return static_cast<typename __builder_t::page_id_t>(i);
        if (tuple.start_vid > vid)
            return static_cast<typename __builder_t::page_id_t>(i - 1);
    }
    return static_cast<typename __builder_t::page_id_t>(table.size()) - 1;
}

//TODO: KNOWN ISSUE: UNSAFE CONVERSION
template <typename __builder_t, typename __rid_table_t>
typename __builder_t::slot_offset_t get_slot_offset(typename __builder_t::page_id_t pid, typename __builder_t::vertex_id_t vid, __rid_table_t& table)
{
    const auto& tuple = table[pid];
    return static_cast<typename __builder_t::slot_offset_t>(vid - tuple.start_vid);
}

template <typename __vertex_id_t, typename __payload_t = void>
struct edge_template
{
    using vertex_id_t = __vertex_id_t;
    using payload_t = __payload_t;
    vertex_id_t src;
    vertex_id_t dst;
    payload_t   payload;
    template <typename __builder_t, typename __rid_table_t>
    void to_adj_elem(const __rid_table_t& table, typename __builder_t::adj_list_elem_t* out)
    {
        out->page_id = vid_to_pid<__builder_t>(dst, table);
        out->slot_offset = get_slot_offset<__builder_t>(out->page_id, dst, table);
        out->payload = payload;
    }
};

template <typename __vertex_id_t>
struct edge_template<__vertex_id_t, void>
{
    using vertex_id_t = __vertex_id_t;
    using payload_t = void;
    vertex_id_t src;
    vertex_id_t dst;
    template <typename __builder_t, typename __rid_table_t>
    void to_adj_elem(const __rid_table_t& table, typename __builder_t::adj_list_elem_t* out)
    {
        out->page_id = vid_to_pid<__builder_t>(dst, table);
        out->slot_offset =  get_slot_offset<__builder_t>(out->page_id, dst, table);
    }
};

template <typename __vertex_id_t, typename __payload_t = void>
struct vertex_template
{
    using vertex_id_t = __vertex_id_t;
    using payload_t = __payload_t;
    vertex_id_t vertex_id;
    payload_t   payload;
    template <typename __builder_t>
    void to_slot(__builder_t& target_page) const
    {
        target_page.add_slot(vertex_id, payload);
    }
    template <typename __builder_t>
    void to_slot_ext(__builder_t& target_page) const
    {
        target_page.add_slot_ext(vertex_id, payload);
    }
};

template <typename __vertex_id_t>
struct vertex_template<__vertex_id_t, void>
{
    using vertex_id_t = __vertex_id_t;
    using payload_t = void;
    vertex_id_t vertex_id;
    template <typename __builder_t>
    void to_slot(__builder_t& target_page) const
    {
        target_page.add_slot(vertex_id);
    }
    template <typename __builder_t>
    void to_slot_ext(__builder_t& target_page) const
    {
        target_page.add_slot_ext(vertex_id);
    }
};

#pragma pack(pop)

} // !namespace igraph

#endif // !_INFORGRAPH_TYPE_SLOTTED_PAGE_H_