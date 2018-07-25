//////////////////////////////////////////////////////////////////////////
//                                                                      //
//            ������� ����������� ����������� ���������������           //
//                                                                      //
//       ���������� ������� � ���������� ���������� ������� DWARF       //
//                                                                      //
//               �������������� ������ pro_garn.cpp                     //
//                                                                      //
//  ���� ���������  ������� Dwarf_P_Debug::transform_arange_to_disk     //
//                                                                      //
//                                                                                         //
//  If this code works, it was                                                         //
//  If not, I don't know who wrote it.                                  //
//                                                                      //
// $Revision:: 1     $      $Date:: 7/21/05 5:33p    $                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------
//
// class Dwarf_P_Arange
//
//---------------------------------------------------------

#include <string.h>

#include "libelf.h"
#include "dwarferr.h"
#include "dwarftyp.h"
#include "dwarf.h"
#include "libdwarf.h"
#include "dwarfint.h"

#include "pro_aran.h"
#include "pro_dbg.h"

Dwarf_Signed Dwarf_P_Debug::transform_arange_to_disk(Dwarf_P_CU *cu) throw(Dwarf_Error)
{
		 // ����� ����� ������ � ������ .debug_aranges
	Dwarf_Unsigned arange_num_bytes;

		 // ����� ������ ������� ���� ��������� � ������ ����� ����������
		 // ����� ��������� �� ������� ���� �������
	Dwarf_Small remainder;

		 // ����� ����� ������ �������� ���� ���� �����
	Dwarf_Unsigned adjusted_length;

		 // ����� ����� ������ � ������ .rel.debug_aranges
	Dwarf_Unsigned arange_reloc_num_bytes;

		 // ��������� �� ������ ���� ������ ��� ������ .debug_aranges
	Dwarf_Small *arange;

		 // ��������� ������������ ��� ���������� ������ .debug_aranges
	Dwarf_Small *arange_ptr;

		 // ��������� �� ������ ���� ������ ��� ������ .rel.debug_aranges
	Dwarf_Small *arange_reloc;

		 // ��������� ������������ ��� ���������� ������ .rel.debug_aranges
	Dwarf_Small *arange_reloc_ptr;

		 // ��������� ��� ������������ ������ �������� ����������
	Dwarf_P_Arange *given_arange;

		 // ������������ ����� ����� �������� 0 � �����
	const Dwarf_Unsigned  big_zero = 0;

		 // ������ �����������. ������������ �����
	Elf32_Rel *elf32_reloc;

      // ������ ������� ���������������� ������ .rel.debug_arange
	Dwarf_Unsigned name_idx;

      // ��������� ���������� ��� ������ 4-� �������� ������� � ����
   Dwarf_Unsigned dw;

	// -------- BEGIN CODE ----------

   if (cu->dpc_arange == NULL)  // ���� ��� �������� �� �� ��������
      return dpd_n_debug_sect;

      // ������ ��������� ������ ������ .debug_aranges
   arange_num_bytes =   word_size() +   // ������ ���� �����.
                        half_size() +   // ������ ���� ������.
                        word_size() +   // ������ �������� � .debug_info.
                        byte_size() +   // ������ ���� ������� ������.
                        byte_size();    // ������ ���� ������� ��������.

      // ����������� ������ ���, ��� ���������� �� �������� ����������
      // ���������� �� �������� ������������ �� ������� �������� ������
      // ��� ������� �������� DWARF v2.

   remainder = Dwarf_Small(arange_num_bytes % (2 * addr_size()));
   if (remainder != 0)
      arange_num_bytes += ( 2 * addr_size() ) - remainder;

      // ��������� ����� ��� ����� �������� ����������
   arange_num_bytes += word_size() * 2 * (cu->dpc_arange_count + 1);

      // �������� ������
   arange = get_new_chunk (Dwarf::elf_sects[DEBUG_ARANGES], arange_num_bytes);
   arange_ptr = arange;

      // ���������� ����� ����� ������ .debug_aranges
   adjusted_length = arange_num_bytes - word_size();
   write_4(&adjusted_length, arange_ptr);
   arange_ptr += word_size();

      // ���������� ������ .

   Dwarf_Half dh = CURRENT_VERSION_STAMP;
   write_2(&dh,arange_ptr);

   arange_ptr += half_size();

      // ���������� ��������� � ������ .debug_info

   write_4(&(cu->dpc_info_offset),arange_ptr);
   arange_ptr += word_size();

      // ���������� ������ ������
   *arange_ptr = Dwarf_Small(addr_size());  // �� ��������������� ����� ������ 32 ����
   arange_ptr++;

       // ���������� ������ ���������� ����� ������.
       // �� ��������������� ������ 0.
   *arange_ptr = 0;
   arange_ptr++;

       // ���������� ����� ��� ������������ ������ ���������� �� ����������
       // �� ������� �������� ������� ������
   if (remainder != 0)
       arange_ptr += (2*addr_size()) - remainder;

   arange_reloc_num_bytes = ELF32_T_REL_FSIZE * (cu->dpc_arange_count + 1);

   Dwarf::reloc_sects[DEBUG_ARANGES] = dpd_func(".rel.debug_aranges", SHT_REL, 0,
                     SHN_UNDEF, Dwarf::elf_sects[DEBUG_ARANGES], &name_idx);

   if (Dwarf::reloc_sects[DEBUG_ARANGES] == -1)
		throw Dwarf_Error_Pro_Elf("error creating section .rel.debug_aranges");

   arange_reloc = get_new_chunk ( Dwarf::reloc_sects[DEBUG_ARANGES],
                                 arange_reloc_num_bytes);

   arange_reloc_ptr = arange_reloc;

      // ������� ������ � ������� ����������� ��� �������� � ������ .debug_info
   elf32_reloc = (Elf32_Rel *)arange_reloc_ptr;
   elf32_reloc->r_offset = cu->dpc_info_offset + word_size() + half_size();
   elf32_reloc->r_info = ELF32_R_INFO(Elf32_Half(Dwarf::sect_name_idx[DEBUG_INFO]), R_NC_BYTEPTR);

   arange_reloc_ptr += ELF32_T_REL_FSIZE;

   for (given_arange = cu->dpc_arange; given_arange != NULL;
               given_arange = given_arange->next) {

         // ������� ������ � ������� ����������� ��� ������ ������ ���������
      elf32_reloc = (Elf32_Rel *)arange_reloc_ptr;
      elf32_reloc->r_offset = arange_ptr - arange; // ���� ����� 1 CU, �� ����������
      elf32_reloc->r_info = ELF32_R_INFO(Elf32_Half(given_arange->sym_idx()), R_NC_ADDR);
      arange_reloc_ptr += ELF32_T_REL_FSIZE;

           // ���������� ������ ���������
      dw = given_arange->addr();
      write_4(&dw, arange_ptr);
      arange_ptr += word_size();

           // ���������� ����� ���������
      dw = given_arange->len();
      write_4(&dw, arange_ptr);
      arange_ptr += word_size();
   }

   write_4(&big_zero, arange_ptr);
   arange_ptr += word_size();
   write_4(&big_zero, arange_ptr);

	return dpd_n_debug_sect;
}
