#include "stdafx.h"
#include "PackAggregate.h"
#include "md5ex.h"
#include "mydes.h"
#include "CCFileUtils.h"
#include "CCFileUtilsWin32.h"
#include "CCData.h"

const char c_szPackMagic[] = "XPAK";
const DWORD c_dwPackVersion = 9;

static t_DES_SPtrans Data_DES_SPtrans = {
	{
		/* nibble 0 */
		0x02080800L, 0x00080000L, 0x02000002L, 0x02080802L,
		0x02000000L, 0x00080802L, 0x00080002L, 0x02000002L,
		0x00080802L, 0x02080800L, 0x02080000L, 0x00000802L,
		0x02000802L, 0x02000000L, 0x00000000L, 0x00080002L,
		0x00080000L, 0x00000002L, 0x02000800L, 0x00080800L,
		0x02080802L, 0x02080000L, 0x00000802L, 0x02000800L,
		0x00000002L, 0x00000800L, 0x00080800L, 0x02080002L,
		0x00000800L, 0x02000802L, 0x02080002L, 0x00000000L,
		0x00000000L, 0x02080802L, 0x02000800L, 0x00080002L,
		0x02080800L, 0x00080000L, 0x00000802L, 0x02000800L,
		0x02080002L, 0x00000800L, 0x00080800L, 0x02000002L,
		0x00080802L, 0x00000002L, 0x02000002L, 0x02080000L,
		0x02080802L, 0x00080800L, 0x02080000L, 0x02000802L,
		0x02000000L, 0x00000802L, 0x00080002L, 0x00000000L,
		0x00080000L, 0x02000000L, 0x02000802L, 0x02080800L,
		0x00000002L, 0x02080002L, 0x00000800L, 0x00080802L,
	},
	{
		/* nibble 1 */
		0x40108010L, 0x00000000L, 0x00108000L, 0x40100000L,
		0x40000010L, 0x00008010L, 0x40008000L, 0x00108000L,
		0x00008000L, 0x40100010L, 0x00000010L, 0x40008000L,
		0x00100010L, 0x40108000L, 0x40100000L, 0x00000010L,
		0x00100000L, 0x40008010L, 0x40100010L, 0x00008000L,
		0x00108010L, 0x40000000L, 0x00000000L, 0x00100010L,
		0x40008010L, 0x00108010L, 0x40108000L, 0x40000010L,
		0x40000000L, 0x00100000L, 0x00008010L, 0x40108010L,
		0x00100010L, 0x40108000L, 0x40008000L, 0x00108010L,
		0x40108010L, 0x00100010L, 0x40000010L, 0x00000000L,
		0x40000000L, 0x00008010L, 0x00100000L, 0x40100010L,
		0x00008000L, 0x40000000L, 0x00108010L, 0x40008010L,
		0x40108000L, 0x00008000L, 0x00000000L, 0x40000010L,
		0x00000010L, 0x40108010L, 0x00108000L, 0x40100000L,
		0x40100010L, 0x00100000L, 0x00008010L, 0x40008000L,
		0x40008010L, 0x00000010L, 0x40100000L, 0x00108000L,
	},
	{
		/* nibble 2 */
		0x04000001L, 0x04040100L, 0x00000100L, 0x04000101L,
		0x00040001L, 0x04000000L, 0x04000101L, 0x00040100L,
		0x04000100L, 0x00040000L, 0x04040000L, 0x00000001L,
		0x04040101L, 0x00000101L, 0x00000001L, 0x04040001L,
		0x00000000L, 0x00040001L, 0x04040100L, 0x00000100L,
		0x00000101L, 0x04040101L, 0x00040000L, 0x04000001L,
		0x04040001L, 0x04000100L, 0x00040101L, 0x04040000L,
		0x00040100L, 0x00000000L, 0x04000000L, 0x00040101L,
		0x04040100L, 0x00000100L, 0x00000001L, 0x00040000L,
		0x00000101L, 0x00040001L, 0x04040000L, 0x04000101L,
		0x00000000L, 0x04040100L, 0x00040100L, 0x04040001L,
		0x00040001L, 0x04000000L, 0x04040101L, 0x00000001L,
		0x00040101L, 0x04000001L, 0x04000000L, 0x04040101L,
		0x00040000L, 0x04000100L, 0x04000101L, 0x00040100L,
		0x04000100L, 0x00000000L, 0x04040001L, 0x00000101L,
		0x04000001L, 0x00040101L, 0x00000100L, 0x04040000L,
	},
	{
		/* nibble 3 */
		0x00401008L, 0x10001000L, 0x00000008L, 0x10401008L,
		0x00000000L, 0x10400000L, 0x10001008L, 0x00400008L,
		0x10401000L, 0x10000008L, 0x10000000L, 0x00001008L,
		0x10000008L, 0x00401008L, 0x00400000L, 0x10000000L,
		0x10400008L, 0x00401000L, 0x00001000L, 0x00000008L,
		0x00401000L, 0x10001008L, 0x10400000L, 0x00001000L,
		0x00001008L, 0x00000000L, 0x00400008L, 0x10401000L,
		0x10001000L, 0x10400008L, 0x10401008L, 0x00400000L,
		0x10400008L, 0x00001008L, 0x00400000L, 0x10000008L,
		0x00401000L, 0x10001000L, 0x00000008L, 0x10400000L,
		0x10001008L, 0x00000000L, 0x00001000L, 0x00400008L,
		0x00000000L, 0x10400008L, 0x10401000L, 0x00001000L,
		0x10000000L, 0x10401008L, 0x00401008L, 0x00400000L,
		0x10401008L, 0x00000008L, 0x10001000L, 0x00401008L,
		0x00400008L, 0x00401000L, 0x10400000L, 0x10001008L,
		0x00001008L, 0x10000000L, 0x10000008L, 0x10401000L,
	},
	{
		/* nibble 4 */
		0x08000000L, 0x00010000L, 0x00000400L, 0x08010420L,
		0x08010020L, 0x08000400L, 0x00010420L, 0x08010000L,
		0x00010000L, 0x00000020L, 0x08000020L, 0x00010400L,
		0x08000420L, 0x08010020L, 0x08010400L, 0x00000000L,
		0x00010400L, 0x08000000L, 0x00010020L, 0x00000420L,
		0x08000400L, 0x00010420L, 0x00000000L, 0x08000020L,
		0x00000020L, 0x08000420L, 0x08010420L, 0x00010020L,
		0x08010000L, 0x00000400L, 0x00000420L, 0x08010400L,
		0x08010400L, 0x08000420L, 0x00010020L, 0x08010000L,
		0x00010000L, 0x00000020L, 0x08000020L, 0x08000400L,
		0x08000000L, 0x00010400L, 0x08010420L, 0x00000000L,
		0x00010420L, 0x08000000L, 0x00000400L, 0x00010020L,
		0x08000420L, 0x00000400L, 0x00000000L, 0x08010420L,
		0x08010020L, 0x08010400L, 0x00000420L, 0x00010000L,
		0x00010400L, 0x08010020L, 0x08000400L, 0x00000420L,
		0x00000020L, 0x00010420L, 0x08010000L, 0x08000020L,
	},
	{
		/* nibble 5 */
		0x80000040L, 0x00200040L, 0x00000000L, 0x80202000L,
		0x00200040L, 0x00002000L, 0x80002040L, 0x00200000L,
		0x00002040L, 0x80202040L, 0x00202000L, 0x80000000L,
		0x80002000L, 0x80000040L, 0x80200000L, 0x00202040L,
		0x00200000L, 0x80002040L, 0x80200040L, 0x00000000L,
		0x00002000L, 0x00000040L, 0x80202000L, 0x80200040L,
		0x80202040L, 0x80200000L, 0x80000000L, 0x00002040L,
		0x00000040L, 0x00202000L, 0x00202040L, 0x80002000L,
		0x00002040L, 0x80000000L, 0x80002000L, 0x00202040L,
		0x80202000L, 0x00200040L, 0x00000000L, 0x80002000L,
		0x80000000L, 0x00002000L, 0x80200040L, 0x00200000L,
		0x00200040L, 0x80202040L, 0x00202000L, 0x00000040L,
		0x80202040L, 0x00202000L, 0x00200000L, 0x80002040L,
		0x80000040L, 0x80200000L, 0x00202040L, 0x00000000L,
		0x00002000L, 0x80000040L, 0x80002040L, 0x80202000L,
		0x80200000L, 0x00002040L, 0x00000040L, 0x80200040L,
	},
	{
		/* nibble 6 */
		0x00004000L, 0x00000200L, 0x01000200L, 0x01000004L,
		0x01004204L, 0x00004004L, 0x00004200L, 0x00000000L,
		0x01000000L, 0x01000204L, 0x00000204L, 0x01004000L,
		0x00000004L, 0x01004200L, 0x01004000L, 0x00000204L,
		0x01000204L, 0x00004000L, 0x00004004L, 0x01004204L,
		0x00000000L, 0x01000200L, 0x01000004L, 0x00004200L,
		0x01004004L, 0x00004204L, 0x01004200L, 0x00000004L,
		0x00004204L, 0x01004004L, 0x00000200L, 0x01000000L,
		0x00004204L, 0x01004000L, 0x01004004L, 0x00000204L,
		0x00004000L, 0x00000200L, 0x01000000L, 0x01004004L,
		0x01000204L, 0x00004204L, 0x00004200L, 0x00000000L,
		0x00000200L, 0x01000004L, 0x00000004L, 0x01000200L,
		0x00000000L, 0x01000204L, 0x01000200L, 0x00004200L,
		0x00000204L, 0x00004000L, 0x01004204L, 0x01000000L,
		0x01004200L, 0x00000004L, 0x00004004L, 0x01004204L,
		0x01000004L, 0x01004200L, 0x01004000L, 0x00004004L,
	},
	{
		/* nibble 7 */
		0x20800080L, 0x20820000L, 0x00020080L, 0x00000000L,
		0x20020000L, 0x00800080L, 0x20800000L, 0x20820080L,
		0x00000080L, 0x20000000L, 0x00820000L, 0x00020080L,
		0x00820080L, 0x20020080L, 0x20000080L, 0x20800000L,
		0x00020000L, 0x00820080L, 0x00800080L, 0x20020000L,
		0x20820080L, 0x20000080L, 0x00000000L, 0x00820000L,
		0x20000000L, 0x00800000L, 0x20020080L, 0x20800080L,
		0x00800000L, 0x00020000L, 0x20820000L, 0x00000080L,
		0x00800000L, 0x00020000L, 0x20000080L, 0x20820080L,
		0x00020080L, 0x20000000L, 0x00000000L, 0x00820000L,
		0x20800080L, 0x20020080L, 0x20020000L, 0x00800080L,
		0x20820000L, 0x00000080L, 0x00800080L, 0x20020000L,
		0x20820080L, 0x00800000L, 0x20800000L, 0x20000080L,
		0x00820000L, 0x00020080L, 0x20020080L, 0x20800000L,
		0x00000080L, 0x20820000L, 0x00820080L, 0x00000000L,
		0x20000000L, 0x20800080L, 0x00020000L, 0x00820080L,
	}
};


static t_DES_SPtrans HeaderData_DES_SPtrans = {
	{
		/* nibble 0 */
		0x02080800L, 0x00080000L, 0x02000002L, 0x02080802L,
		0x02000000L, 0x00080802L, 0x00080002L, 0x02000002L,
		0x00080802L, 0x02080800L, 0x02080000L, 0x00000802L,
		0x02080802L, 0x00080800L, 0x02080000L, 0x02000802L,

		0x02000802L, 0x02000000L, 0x00000000L, 0x00080002L,

		0x00080000L, 0x00000002L, 0x02000800L, 0x00080800L,

		0x02080802L, 0x02080000L, 0x00000802L, 0x02000800L,

		0x00000002L, 0x00000800L, 0x00080800L, 0x02080002L,

		0x00000800L, 0x02000802L, 0x02080002L, 0x00000000L,

		0x00000000L, 0x02080802L, 0x02000800L, 0x00080002L,

		0x02080800L, 0x00080000L, 0x00000802L, 0x02000800L,

		0x02080002L, 0x00000800L, 0x00080800L, 0x02000002L,

		0x00080802L, 0x00000002L, 0x02000002L, 0x02080000L,

		0x02000000L, 0x00000802L, 0x00080002L, 0x00000000L,
		0x00080000L, 0x02000000L, 0x02000802L, 0x02080800L,
		0x00000002L, 0x02080002L, 0x00000800L, 0x00080802L,
	},
	{
		/* nibble 1 */
		0x40108010L, 0x00000000L, 0x00108000L, 0x40100000L,
		0x40000010L, 0x00008010L, 0x40008000L, 0x00108000L,
		0x00008000L, 0x40100010L, 0x00000010L, 0x40008000L,
		0x00100010L, 0x40108000L, 0x40100000L, 0x00000010L,
		0x00100000L, 0x40008010L, 0x40100010L, 0x00008000L,
		0x00108010L, 0x40000000L, 0x00000000L, 0x00100010L,
		0x40008010L, 0x00108010L, 0x40108000L, 0x40000010L,
		0x40000000L, 0x00100000L, 0x00008010L, 0x40108010L,
		0x00100010L, 0x40108000L, 0x40008000L, 0x00108010L,
		0x40108010L, 0x00100010L, 0x40000010L, 0x00000000L,
		0x40000000L, 0x00008010L, 0x00100000L, 0x40100010L,
		0x00008000L, 0x40000000L, 0x00108010L, 0x40008010L,
		0x40108000L, 0x00008000L, 0x00000000L, 0x40000010L,
		0x00000010L, 0x40108010L, 0x00108000L, 0x40100000L,
		0x40100010L, 0x00100000L, 0x00008010L, 0x40008000L,
		0x40008010L, 0x00000010L, 0x40100000L, 0x00108000L,
	},
	{
		/* nibble 2 */
		0x04000001L, 0x04040100L, 0x00000100L, 0x04000101L,
		0x00040001L, 0x04000000L, 0x04000101L, 0x00040100L,
		0x04000100L, 0x00040000L, 0x04040000L, 0x00000001L,
		0x04040101L, 0x00000101L, 0x00000001L, 0x04040001L,
		0x00000000L, 0x00040001L, 0x04040100L, 0x00000100L,
		0x00000101L, 0x04040101L, 0x00040000L, 0x04000001L,

		0x00040101L, 0x04000001L, 0x04000000L, 0x04040101L,

		0x04040001L, 0x04000100L, 0x00040101L, 0x04040000L,

		0x00040100L, 0x00000000L, 0x04000000L, 0x00040101L,

		0x04040100L, 0x00000100L, 0x00000001L, 0x00040000L,

		0x00000101L, 0x00040001L, 0x04040000L, 0x04000101L,

		0x00000000L, 0x04040100L, 0x00040100L, 0x04040001L,

		0x00040001L, 0x04000000L, 0x04040101L, 0x00000001L,

		0x00040000L, 0x04000100L, 0x04000101L, 0x00040100L,
		0x04000100L, 0x00000000L, 0x04040001L, 0x00000101L,
		0x04000001L, 0x00040101L, 0x00000100L, 0x04040000L,
	},
	{
		/* nibble 3 */
		0x00401008L, 0x10001000L, 0x00000008L, 0x10401008L,
		0x00000000L, 0x10400000L, 0x10001008L, 0x00400008L,

		0x10000008L, 0x00401008L, 0x00400000L, 0x10000000L,

		0x10400008L, 0x00401000L, 0x00001000L, 0x00000008L,

		0x00401000L, 0x10001008L, 0x10400000L, 0x00001000L,

		0x00001008L, 0x00000000L, 0x00400008L, 0x10401000L,

		0x10001000L, 0x10400008L, 0x10401008L, 0x00400000L,

		0x10400008L, 0x00001008L, 0x00400000L, 0x10000008L,

		0x00000020L, 0x08000420L, 0x08010420L, 0x00010020L,

		0x00401000L, 0x10001000L, 0x00000008L, 0x10400000L,
		0x10001008L, 0x00000000L, 0x00001000L, 0x00400008L,
		0x00000000L, 0x10400008L, 0x10401000L, 0x00001000L,
		0x10000000L, 0x10401008L, 0x00401008L, 0x00400000L,
		0x10401008L, 0x00000008L, 0x10001000L, 0x00401008L,
		0x00400008L, 0x00401000L, 0x10400000L, 0x10001008L,
		0x00001008L, 0x10000000L, 0x10000008L, 0x10401000L,
	},
	{
		/* nibble 5 */
		0x80000040L, 0x00200040L, 0x00000000L, 0x80202000L,
		0x00200040L, 0x00002000L, 0x80002040L, 0x00200000L,
		0x00002040L, 0x80202040L, 0x00202000L, 0x80000000L,

		0x00000040L, 0x00202000L, 0x00202040L, 0x80002000L,

		0x00002040L, 0x80000000L, 0x80002000L, 0x00202040L,

		0x80002000L, 0x80000040L, 0x80200000L, 0x00202040L,

		0x00200000L, 0x80002040L, 0x80200040L, 0x00000000L,

		0x00002000L, 0x00000040L, 0x80202000L, 0x80200040L,

		0x80202040L, 0x80200000L, 0x80000000L, 0x00002040L,

		0x80202000L, 0x00200040L, 0x00000000L, 0x80002000L,

		0x80000000L, 0x00002000L, 0x80200040L, 0x00200000L,
		0x00200040L, 0x80202040L, 0x00202000L, 0x00000040L,
		0x80202040L, 0x00202000L, 0x00200000L, 0x80002040L,
		0x80000040L, 0x80200000L, 0x00202040L, 0x00000000L,
		0x00002000L, 0x80000040L, 0x80002040L, 0x80202000L,
		0x80200000L, 0x00002040L, 0x00000040L, 0x80200040L,
	},
	{
		/* nibble 6 */
		0x00004000L, 0x00000200L, 0x01000200L, 0x01000004L,
		0x01004204L, 0x00004004L, 0x00004200L, 0x00000000L,
		0x01000000L, 0x01000204L, 0x00000204L, 0x01004000L,
		0x00000004L, 0x01004200L, 0x01004000L, 0x00000204L,
		0x01000204L, 0x00004000L, 0x00004004L, 0x01004204L,
		0x00000000L, 0x01000200L, 0x01000004L, 0x00004200L,
		0x01004004L, 0x00004204L, 0x01004200L, 0x00000004L,
		0x00004204L, 0x01004004L, 0x00000200L, 0x01000000L,
		0x00004204L, 0x01004000L, 0x01004004L, 0x00000204L,
		0x00004000L, 0x00000200L, 0x01000000L, 0x01004004L,
		0x01000204L, 0x00004204L, 0x00004200L, 0x00000000L,
		0x00000200L, 0x01000004L, 0x00000004L, 0x01000200L,
		0x00000000L, 0x01000204L, 0x01000200L, 0x00004200L,
		0x00000204L, 0x00004000L, 0x01004204L, 0x01000000L,
		0x01004200L, 0x00000004L, 0x00004004L, 0x01004204L,
		0x01000004L, 0x01004200L, 0x01004000L, 0x00004004L,
	},
	{
		/* nibble 4 */
		0x08000000L, 0x00010000L, 0x00000400L, 0x08010420L,

		0x10401000L, 0x10000008L, 0x10000000L, 0x00001008L,

		0x08010020L, 0x08000400L, 0x00010420L, 0x08010000L,

		0x00010000L, 0x00000020L, 0x08000020L, 0x00010400L,

		0x08000420L, 0x08010020L, 0x08010400L, 0x00000000L,

		0x00010400L, 0x08000000L, 0x00010020L, 0x00000420L,

		0x08000400L, 0x00010420L, 0x00000000L, 0x08000020L,

		0x08010000L, 0x00000400L, 0x00000420L, 0x08010400L,

		0x08010400L, 0x08000420L, 0x00010020L, 0x08010000L,
		0x00010000L, 0x00000020L, 0x08000020L, 0x08000400L,
		0x08000000L, 0x00010400L, 0x08010420L, 0x00000000L,
		0x00010420L, 0x08000000L, 0x00000400L, 0x00010020L,
		0x08000420L, 0x00000400L, 0x00000000L, 0x08010420L,
		0x08010020L, 0x08010400L, 0x00000420L, 0x00010000L,
		0x00010400L, 0x08010020L, 0x08000400L, 0x00000420L,
		0x00000020L, 0x00010420L, 0x08010000L, 0x08000020L,
	},
	{
		/* nibble 7 */
		0x20800080L, 0x20820000L, 0x00020080L, 0x00000000L,
		0x20020000L, 0x00800080L, 0x20800000L, 0x20820080L,
		0x00000080L, 0x20000000L, 0x00820000L, 0x00020080L,
		0x00820080L, 0x20020080L, 0x20000080L, 0x20800000L,
		0x00020000L, 0x00820080L, 0x00800080L, 0x20020000L,
		0x20820080L, 0x20000080L, 0x00000000L, 0x00820000L,
		0x20000000L, 0x00800000L, 0x20020080L, 0x20800080L,
		0x00800000L, 0x00020000L, 0x20820000L, 0x00000080L,
		0x00800000L, 0x00020000L, 0x20000080L, 0x20820080L,
		0x00020080L, 0x20000000L, 0x00000000L, 0x00820000L,
		0x20800080L, 0x20020080L, 0x20020000L, 0x00800080L,
		0x20820000L, 0x00000080L, 0x00800080L, 0x20020000L,
		0x20820080L, 0x00800000L, 0x20800000L, 0x20000080L,
		0x00820000L, 0x00020080L, 0x20020080L, 0x20800000L,
		0x00000080L, 0x20820000L, 0x00820080L, 0x00000000L,
		0x20000000L, 0x20800080L, 0x00020000L, 0x00820080L,
	}
};

void PasswordToDesKey(const char* pszPassword, BYTE* key1, BYTE* key2, BYTE* key3)
{
	BYTE md5[16];
	if (pszPassword)
	{
		MD5String(pszPassword, md5);
		memcpy(key1, md5, 8);
		memcpy(key2, md5 + 8, 8);
		memcpy(key3, md5 + 4, 8);
	}
}
void DecryptData_model(BYTE * inData, size_t len, const BYTE* key1, const BYTE* key2, const BYTE* key3, t_DES_SPtrans * sp)
{
	assert((len & 0x7) == 0);
	DES_key_schedule sKey[3];

	DES_set_key((const_DES_cblock*)key1, &sKey[0]);
	DES_set_key((const_DES_cblock*)key2, &sKey[1]);
	DES_set_key((const_DES_cblock*)key3, &sKey[2]);

	for (size_t i = 0; i < len; i += 8)
	{
		DES_decrypt3((DWORD*)(inData + i), &sKey[0], &sKey[1], &sKey[2], sp);
	}
}

void DecryptData(BYTE * inData, size_t len, const BYTE* key1, const BYTE* key2, const BYTE* key3)
{
	DecryptData_model(inData, len, key1, key2, key3, &Data_DES_SPtrans);
}

void EncryptData_model(BYTE * inData, size_t len, const BYTE* key1, const BYTE* key2, const BYTE* key3, t_DES_SPtrans * sp)
{
	assert((len & 0x7) == 0);
#ifdef _DEBUG
	BYTE * oldData = new BYTE[len];
	memcpy(oldData, inData, len);
#endif
	DES_key_schedule sKey[3];

	DES_set_key((const_DES_cblock*)key1, &sKey[0]);
	DES_set_key((const_DES_cblock*)key2, &sKey[1]);
	DES_set_key((const_DES_cblock*)key3, &sKey[2]);

	for (size_t i = 0; i < len; i += 8)
	{
		DES_encrypt3((DWORD*)(inData + i), &sKey[0], &sKey[1], &sKey[2], sp);
	}
#ifdef _DEBUG
	BYTE* newData = new BYTE[len];
	memcpy(newData, inData, len);
	DecryptData(newData, len, key1, key2, key3);
	int cmp = memcmp(newData, oldData, len);
	assert(cmp == 0);
	CC_SAFE_DELETE_ARRAY(newData);
	CC_SAFE_DELETE_ARRAY(oldData);
#endif
}

void EncryptData(BYTE * inData, size_t len, const BYTE* key1, const BYTE* key2, const BYTE* key3)
{
	EncryptData_model(inData, len, key1, key2, key3, &Data_DES_SPtrans);
}



void DecryptHeaderData(BYTE * inData, size_t len, const BYTE* key1, const BYTE* key2, const BYTE* key3)
{
	DecryptData_model(inData, len, key1, key2, key3, &HeaderData_DES_SPtrans);
}

void EncryptHeaderData(BYTE * inData, size_t len, const BYTE* key1, const BYTE* key2, const BYTE* key3)
{
	EncryptData_model(inData, len, key1, key2, key3, &HeaderData_DES_SPtrans);
}

PackAggregate::PackAggregate()
{
	m_szFileName[0] = 0;
	m_dwFlags = 0;
	m_pDesKey = NULL;
}


PackAggregate::~PackAggregate()
{
}

void PackAggregate::closeAggregate()
{
	m_aFileInfo.resize(0);
	m_szFileName[0] = 0;
}

bool PackAggregate::refreshAggregate()
{
	//CC_ASSERT(m_szFileName[0] != 0, "No filename? Must not be open. Disallowed");
	CC_ASSERT(m_szFileName[0] != 0);
	char tmpBuff[MAX_PATH] = { 0 };
	strcpy(tmpBuff, m_szFileName);
	//return openAggregate(tmpBuff) == OpenOk;
	return true;
}

const char* PackAggregate::GetDefaultPassword()
{
	static char pszPassword[] = "xuzhao20041108";
	return pszPassword;
}

bool PackAggregate::resortFilePack(const char* pack, std::vector<PackAggregate::stFileInfo> & files)
{
	char szDir[MAX_PATH] = { 0 };
	char tmpFile[MAX_PATH] = { 0 };
	char *p;
	strcpy(szDir, pack);
	p = strrchr(szDir, '\\');
	if (p)
		*p = 0;
	if (!GetTempFileNameA(szDir, "pak", 0, tmpFile))	//����0�ֽ��ļ���ΪszDir.pak���ļ�
	{
		return false;
	}

	if (!CopyFileA(pack, tmpFile, FALSE))	//���ļ���������ʱ�ļ���
		return false;

	Data mdata = FileUtils::getInstance()->getDataFromFile(tmpFile);
	PackAggregate::stPackHeader hdr;
	mdata.read(&hdr, sizeof(PackAggregate::stPackHeader));

	size_t filesize = mdata.getSize();
	mdata.setPosition(filesize - hdr.headerSize);

	std::vector<BYTE> tmpBuff;
	tmpBuff.resize(files.size() * sizeof(stFileInfo));
	p = (char*)&tmpBuff[0];
	for (size_t i = 0; i < files.size(); ++i)
	{
		stFileInfo1 * dst1 = (stFileInfo1*)p;
		stFileInfo & st = files[i];
		p += sizeof(stFileInfo1);

		*dst1 = st.st1;

		char* pname = (char*)p;
		strcpy(pname, st.szName);
		p += strlen(pname) + 1;
	}

	size_t size = ((p - (char*)&tmpBuff[0]) + 7) & (~7);

	BYTE des_key[3][8];
	PasswordToDesKey(PackAggregate::GetDefaultPassword(), des_key[0], des_key[1], des_key[2]);
	EncryptHeaderData(&tmpBuff[0], size, des_key[0], des_key[1], des_key[2]);
	mdata.write(&tmpBuff[0], size);
	FileUtils::getInstance()->writeDataToFile(tmpFile, mdata);
	return FileUtilsWin32::updateFile(tmpFile, pack);
}

bool PackAggregate::getFileInfo(const char * szFileName, std::vector<stFileInfo> & files)
{
	Data mdata = FileUtils::getInstance()->getDataFromFile(szFileName);
	PackAggregate::stPackHeader hdr;
	mdata.read(&hdr, sizeof(PackAggregate::stPackHeader));

	size_t filesize = mdata.getSize();

	if (strncmp((char*)&hdr.magic, c_szPackMagic, 4))
		return false;

	if (hdr.version != c_dwPackVersion && hdr.version != 8)
		return false;

	mdata.setPosition(filesize - hdr.headerSize);

	std::vector<BYTE> aBuff;
	aBuff.reserve(hdr.headerSize);
	aBuff.resize(hdr.headerSize);
	mdata.read(&aBuff[0], hdr.headerSize);


	BYTE des_key[3][8];
	PasswordToDesKey(PackAggregate::GetDefaultPassword(), des_key[0], des_key[1], des_key[2]);

	if (hdr.flag & flagEncrypt || (hdr.flag & flagEncryptHeaderOnly))
	{
		DecryptHeaderData(
			(BYTE*)&aBuff[0],
			hdr.headerSize,
			des_key[0], 
			des_key[1], 
			des_key[2]
			);
	}

	BYTE * p = &aBuff[0];
	files.resize(hdr.filenum);
	for (size_t i = 0; i < hdr.filenum; ++i)
	{
		stFileInfo & st = files[i];
		memcpy(&st.st1, p, sizeof(stFileInfo1));
		p += sizeof(stFileInfo1);
		strcpy(st.szName, (char*)p);
		p += strlen(st.szName) + 1;
	}

	return true;
}

bool CheckFileInfo(PackAggregate::stFileInfo1* p)
{
	return p->interspaceSize >= p->size && p->fileOffset < (DWORD)1024 * 1024 * 1024 && p->size < (DWORD)256 * 1024 * 1024;
}


PackAggregate::enumOpenResult PackAggregate::openAggregate(const char * szFileName)
{
	closeAggregate();
	Data mdata = FileUtils::getInstance()->getDataFromFile(szFileName);
	size_t filesize = mdata.getSize();
	PackAggregate::stPackHeader hdr;
	mdata.read(&hdr, sizeof(PackAggregate::stPackHeader));

	if (strncmp((char*)&hdr.magic, c_szPackMagic, 4))
		return OpenUnkFormat;

	if (hdr.version != c_dwPackVersion && hdr.version != 8)
		return OpenVersionError;

	if (hdr.headerSize == 0 || hdr.headerSize > 1024 * 1024 * 2)
	{
		return OpenDataError;
	}

	m_dwFlags = hdr.flag;
	m_dwVersion = hdr.version;

	strcpy(m_szFileName, szFileName);
	mdata.setPosition(filesize - hdr.headerSize);

	std::vector<BYTE> aBuff;
	aBuff.reserve(hdr.headerSize);
	aBuff.resize(hdr.headerSize);
	mdata.read(&aBuff[0], hdr.headerSize);

	if (hdr.flag & flagEncrypt || (hdr.flag & flagEncryptHeaderOnly))
	{
		assert(m_pDesKey);
		DecryptHeaderData(
			(BYTE*)&aBuff[0],
			hdr.headerSize,
			m_pDesKey, m_pDesKey + 8, m_pDesKey + 16
			);
	}

	m_aFileInfo.resize(hdr.filenum);
	BYTE *p = &aBuff[0];
	for (size_t i = 0; i < hdr.filenum; ++i)
	{
		stFileInfo st;
		stFileBaseInfo &dst = m_aFileInfo[i];
		memcpy(&st.st1, p, sizeof(stFileInfo1));
		if (!CheckFileInfo(&st.st1))
			return OpenDataError;

		p += sizeof(stFileInfo1);
		if (!strncpy(st.szName, (char*)p, sizeof(st.szName)))
			return OpenDataError;

		p += strlen(st.szName) + 1;

		dst.size = st.size;
		dst.fileOffset = st.fileOffset + sizeof(hdr);
		dst.dwCRC = st.dwCRC;
		strcpy(dst.szName, st.szName);
	}

	return OpenOk;
}