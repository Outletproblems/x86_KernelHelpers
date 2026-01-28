#pragma once

#include <cstdint>

/* Through the intel SDM

To walk a virtual address to physical address there are 3 possibilities assuming this address exists:
PML4 -> PDPTE -> PDE -> PTE      -> PE for a 4KB page
PML4 -> PDPTE -> PDE With PS = 1 -> PE for a 2MB page
PML4 -> PDPTE With PS = 1        -> PE for a 1GB page

The present bit should be checked for every entry value read, for example for a PDPTE:
if(PDPTE & PSE::PDPTE::PRESENT) is false, it implies the entry is invalid

Note also - to get a direct physical address each index needs to be multiplied by 8 to become aligned
GET_ADDRESS() does this automatically, however GET_INDEX() does not.
*/

// Page Structure Entries
namespace PSE
{

	static constexpr std::uint64_t TABLE_BASE_MASK = 0x00'0F'FF'FF'FF'FF'F0'00ULL;

	// Virtual address related 
	namespace VA
	{
		static constexpr std::uint64_t PML4E_INDEX = 39;
		static constexpr std::uint64_t PDPTE_INDEX = 30;
		static constexpr std::uint64_t PDE_INDEX   = 21;
		static constexpr std::uint64_t PTE_INDEX   = 12;

		static constexpr std::uint64_t PSE_MASK = 0x1FFULL;
		static constexpr std::uint64_t PAGE_OFFSET_4KB_MASK = 0x0F'FFULL;
		static constexpr std::uint64_t PAGE_OFFSET_2MB_MASK = 0x1F'FF'FFULL;
		static constexpr std::uint64_t PAGE_OFFSET_1GB_MASK = 0x3F'FF'FF'FFULL;
	}

	// Control register 3
	namespace CR3
	{


		static constexpr std::uint64_t PML4_BASE_MASK = TABLE_BASE_MASK;

		static constexpr std::uint64_t PWT = 1ull << 3;
		static constexpr std::uint64_t PCD = 1ull << 4;

	}

	// Page map level 4 entry
	namespace PML4E
	{
		static constexpr std::uint64_t PDPTE_ADDRESS_MASK = TABLE_BASE_MASK;

		static constexpr std::uint64_t PRESENT = 1ull << 0;
		static constexpr std::uint64_t RW = 1ull << 1;
		static constexpr std::uint64_t US = 1ull << 2;
		static constexpr std::uint64_t PWT = 1ull << 3;
		static constexpr std::uint64_t PCD = 1ull << 4;
		static constexpr std::uint64_t A = 1ull << 5;
		static constexpr std::uint64_t R = 1ull << 11;
		static constexpr std::uint64_t XD = 1ull << 63;

		static inline constexpr std::uint64_t GET_INDEX(std::uint64_t virtual_address)
		{
			return { ((virtual_address >> VA::PML4E_INDEX) & VA::PSE_MASK)};
		}
		static inline constexpr  std::uint64_t GET_TABLE_BASE(std::uint64_t cr3)
		{
			return { cr3 & CR3::PML4_BASE_MASK};
		}
		static inline constexpr std::uint64_t GET_ADDRESS(std::uint64_t cr3, std::uint64_t virtual_address)
		{
			return { GET_TABLE_BASE(cr3) | (GET_INDEX(virtual_address) * 8) };
		}
	}

	// Page directory pointer table entry
	namespace PDPTE
	{
		static constexpr std::uint64_t PDE_ADDRESS_MASK = TABLE_BASE_MASK;

		static constexpr std::uint64_t PRESENT = 1ull << 0;
		static constexpr std::uint64_t RW = 1ull << 1;
		static constexpr std::uint64_t US = 1ull << 2;
		static constexpr std::uint64_t PWT = 1ull << 3;
		static constexpr std::uint64_t PCD = 1ull << 4;
		static constexpr std::uint64_t A = 1ull << 5;
		static constexpr std::uint64_t PS = 1ull << 7;
		static constexpr std::uint64_t R = 1ull << 11;
		static constexpr std::uint64_t XD = 1ull << 63;

		namespace GB
		{
			static constexpr std::uint64_t PHYS_ADDRESS_MASK = 0x00'0F'FF'FF'C0'00'00'00ULL;
			static constexpr std::uint64_t D = 1ull << 6;
			static constexpr std::uint64_t G = 1ull << 8;
			static constexpr std::uint64_t PAT = 1ull << 12;
			static constexpr std::uint64_t PKE = 1ull << 59;

			static inline constexpr std::uint64_t GET_ADDRESS(std::uint64_t pdpte, std::uint64_t virtual_address)
			{
				return { (pdpte & PHYS_ADDRESS_MASK) | (virtual_address & VA::PAGE_OFFSET_1GB_MASK) };
			}
		}
		
		static inline constexpr std::uint64_t GET_INDEX(std::uint64_t virtual_address)
		{
			return { ((virtual_address >> VA::PDPTE_INDEX) & VA::PSE_MASK)};
		}
		static inline constexpr std::uint64_t GET_TABLE_BASE(std::uint64_t pml4e)
		{
			return { pml4e & PML4E::PDPTE_ADDRESS_MASK };
		}
		static inline constexpr std::uint64_t GET_ADDRESS(std::uint64_t pml4e, std::uint64_t virtual_address)
		{
			return { GET_TABLE_BASE(pml4e) | (GET_INDEX(virtual_address) * 8) };
		}
		static inline constexpr bool IS_LARGE_PAGE(std::uint64_t pdpte)
		{
			return (pdpte & PS);
		}
	}

	// Page directory entry
	namespace PDE
	{
		static constexpr std::uint64_t PTE_ADDRESS_MASK = TABLE_BASE_MASK;


		static constexpr std::uint64_t PRESENT = 1ull << 0;
		static constexpr std::uint64_t RW = 1ull << 1;
		static constexpr std::uint64_t US = 1ull << 2;
		static constexpr std::uint64_t PWT = 1ull << 3;
		static constexpr std::uint64_t PCD = 1ull << 4;
		static constexpr std::uint64_t A = 1ull << 5;
		static constexpr std::uint64_t PS = 1ull << 7;
		static constexpr std::uint64_t R = 1ull << 11;
		static constexpr std::uint64_t XD = 1ull << 63;

		namespace MB
		{
			static constexpr std::uint64_t PHYS_ADDRESS_MASK = 0x00'0F'FF'FF'FF'E0'00'00ULL;
			static constexpr std::uint64_t D = 1ull << 6;
			static constexpr std::uint64_t G = 1ull << 8;
			static constexpr std::uint64_t PAT = 1ull << 12;
			static constexpr std::uint64_t PKE = 1ull << 59;

			static inline constexpr std::uint64_t GET_ADDRESS(std::uint64_t pde, std::uint64_t virtual_address)
			{
				return { (pde & PHYS_ADDRESS_MASK) | (virtual_address & VA::PAGE_OFFSET_2MB_MASK) };
			}
		}

		static inline constexpr std::uint64_t GET_INDEX(std::uint64_t virtual_address)
		{
			return { ((virtual_address >> VA::PDE_INDEX) & VA::PSE_MASK)};
		}
		static inline constexpr std::uint64_t GET_TABLE_BASE(std::uint64_t pdpte)
		{
			return { pdpte & PDPTE::PDE_ADDRESS_MASK };
		}
		static inline constexpr std::uint64_t GET_ADDRESS(std::uint64_t pdpte, std::uint64_t virtual_address)
		{
			return { GET_TABLE_BASE(pdpte) | (GET_INDEX(virtual_address) * 8) };
		}
		static inline constexpr bool IS_LARGE_PAGE(std::uint64_t pde)
		{
			return (pde & PS);
		}
	}

	// Page table entry
	namespace PTE
	{
		static constexpr std::uint64_t PHYS_ADDRESS_MASK = TABLE_BASE_MASK;

		static constexpr std::uint64_t PRESENT = 1ull << 0;
		static constexpr std::uint64_t RW = 1ull << 1;
		static constexpr std::uint64_t US = 1ull << 2;
		static constexpr std::uint64_t PWT = 1ull << 3;
		static constexpr std::uint64_t PCD = 1ull << 4;
		static constexpr std::uint64_t A = 1ull << 5;
		static constexpr std::uint64_t D = 1ull << 6;
		static constexpr std::uint64_t PAT = 1ull << 7;
		static constexpr std::uint64_t G = 1ull << 8;
		static constexpr std::uint64_t R = 1ull << 11;
		static constexpr std::uint64_t PKE = 1ull << 59;
		static constexpr std::uint64_t XD = 1ull << 63;

		static inline constexpr std::uint64_t GET_INDEX(std::uint64_t  virtual_address)
		{
			return { ((virtual_address >> VA::PTE_INDEX) & VA::PSE_MASK)};
		}
		static inline constexpr std::uint64_t GET_TABLE_BASE(std::uint64_t pde)
		{
			return { pde & PDE::PTE_ADDRESS_MASK };
		}
		static inline constexpr std::uint64_t GET_ADDRESS(std::uint64_t pde, std::uint64_t virtual_address)
		{
			return { GET_TABLE_BASE(pde) | (GET_INDEX(virtual_address) * 8) };
		}
	}

	// Page entry
	namespace PE
	{
		static inline constexpr std::uint64_t GET_ADDRESS(std::uint64_t pte, std::uint64_t virtual_address)
		{
			return { (pte & PSE::PTE::PHYS_ADDRESS_MASK) | ((virtual_address & PSE::VA::PAGE_OFFSET_4KB_MASK)) };
		}
	}

}
