//////////////////////////////////////////////////////////////////////////
//                                                                      //
//            ������� ����������� ����������� ���������������           //
//                                                                      //
//       ���������� ������� � ���������� ���������� ������� DWARF       //
//                                                                      //
//               �������������� ������ pro_gpub.cpp                     //
//                                                                      //
//  ���� ���������  ������� Dwarf_P_Debug::transform_pubname_to_disk    //
//                                                                      //
//                                                                                         //
//  If this code works, it was                                                         //
//  If not, I don't know who wrote it.                                  //
//                                                                      //
// $Revision:: 1     $      $Date:: 7/21/05 5:33p    $                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------
//
// class Dwarf_P_Pubname to disk transformation
//
//-------------------------------------------------------------------

#include <string.h>

#include "libelf.h"
#include "dwarferr.h"
#include "dwarftyp.h"
#include "dwarf.h"
#include "libdwarf.h"
#include "dwarfint.h"

#include "pro_dbg.h"
#include "pro_pubn.h"

Dwarf_Signed Dwarf_P_Debug::transform_pubname_to_disk(Dwarf_P_CU *cu)
                                                           throw(Dwarf_Error)
{
      // ����� ����� ������ � ������ .debug_pubnames
   Dwarf_Unsigned pubname_num_bytes;

      // ����� ����� ������ �������� ���� �����
   Dwarf_Unsigned adjusted_length;

      // ��������� �� ������ ���� ������ ������ .debug_pubnames
   Dwarf_Small *pubname;

      // ��������� ��� ������������ ������ ��������
   Dwarf_P_Pubname *given_pubname;

      // ��� ���������� ������ ������ ������ .debug_pubnames
   Dwarf_Small *pubname_ptr;

      // ��������� �� ������ ���� ������ ������ .rel.debug_pubnames
   Dwarf_Small *pubname_reloc;

      // ������������ ��� ������ 0
   const Dwarf_Unsigned big_zero = 0;

      // ������������ ��� ������������ ������ ������� ������ ������
   Dwarf_P_Section_Data *debug_sect;

      // ������ ������ .debug_info
   Dwarf_Unsigned debug_info_size;

      // ��������� �� ������ � ������� �����������
   Elf32_Rel *elf32_reloc;

      // ������������ ��� �������� callback ������� �������� ������
   Dwarf_Unsigned name_idx;

   // ---------- BEGIN CODE ----------

   if (cu->dpc_pubname == NULL)  // ���� ��� �������� �� �� ��������
      return dpd_n_debug_sect;

      // ��������� ������ ������ .debug_info
      // �� ����� �������� � ������ ����� ����� CU
   debug_info_size = 0;
   for (debug_sect = dpd_debug_sects; debug_sect != NULL;
       debug_sect = debug_sect->next)
       if (debug_sect->dpsd_elf_sect_no == Dwarf_Unsigned(Dwarf::elf_sects[DEBUG_INFO]))
           debug_info_size += debug_sect->dpsd_nbytes;

       // ������ ��������� ������ .debug_pubnames
   pubname_num_bytes =
       word_size() +     // ������ ���� �����
       half_size() +     // ������ ���� ������
       off_size()  +     // ������ ���� �������� � ������ .debug_info
                         // ���������� ������ CU

       word_size();      // ������ ���� ������� � ������ .debug_info.
                         // ���������� ������ CU

      // Add the Size of the names portion.
   for (given_pubname = cu->dpc_pubname; given_pubname != NULL;
                      given_pubname = given_pubname->next)
      pubname_num_bytes +=
           off_size() +              // ������ �������� DIE
           strlen(given_pubname->name()) + 1; // ������ ����� ����������� �����

       // ��������� ������ ���������� �������� ��������
   pubname_num_bytes += off_size();

   pubname = get_new_chunk(Dwarf::elf_sects[DEBUG_PUBNAMES], pubname_num_bytes);
   pubname_ptr = pubname;

      // ���������� ���������� ����� ������ .debug_pubnames
   adjusted_length = pubname_num_bytes - word_size();
   write_4(&adjusted_length, pubname_ptr);
   pubname_ptr += word_size();

      // ���������� ������ � 2 ������
   Dwarf_Half dh = CURRENT_VERSION_STAMP;
   write_2(&dh, pubname_ptr);

   pubname_ptr += half_size();

      // ���������� �������� ������ CU � ������ .debug_info
   write_4(&cu->dpc_info_offset, pubname_ptr);
   pubname_ptr += off_size();

      // ���������� ������ CU � ������ .debug_info.
   write_4( &debug_info_size, pubname_ptr);
   pubname_ptr += word_size();

   Dwarf::reloc_sects[DEBUG_PUBNAMES] = dpd_func(".rel.debug_pubnames",
            SHT_REL, 0, SHN_UNDEF, Dwarf::elf_sects[DEBUG_PUBNAMES], &name_idx);

   pubname_reloc = get_new_chunk(Dwarf::reloc_sects[DEBUG_PUBNAMES],
                                 sizeof(Elf32_Rel));

      // Write relocation record for .debug_info offset.
   elf32_reloc = (Elf32_Rel *)pubname_reloc;
   elf32_reloc->r_offset = word_size() + half_size();
   elf32_reloc->r_info = ELF32_R_INFO(
            Elf32_Half(Dwarf::sect_name_idx[DEBUG_INFO]),
            R_NC_BYTEPTR);

   for (given_pubname = cu->dpc_pubname; given_pubname != NULL;
      given_pubname = given_pubname->next) {

         // ���������� �������� DIE ������������ ������ CU
      write_4( &(given_pubname->die()->dpd_offset), pubname_ptr);
      pubname_ptr += off_size();

         // ���������� ��� �������
      strcpy((char *)pubname_ptr, given_pubname->name());
      pubname_ptr += strlen(given_pubname->name()) + 1;
   }

   write_4(&big_zero, pubname_ptr);

   return dpd_n_debug_sect;
}
