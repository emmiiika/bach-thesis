#include "implementation.h"

#include <algorithms/isomorphism/isomorphism.hpp>
#include <graphs.hpp>
#include <io/graph6.hpp>
#include <random/renumber.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
	Graph G(empty_graph(2));
	Graph H(empty_graph(2));
	assert(IsomorphismFinder(G, H).are_isomorphic());

	G = std::move(read_graph6_line(":M`?KWAfQpzTPLETtspf"));
	H = std::move(read_graph6_line(":M`?KWAfQpzTPLDTxspf"));
	assert(!are_isomorphic(G, H));

	G = std::move(square_grid(3, 3));
	H = std::move(square_grid(3, 3));
	randomize_vertices(H);
	assert(IsomorphismFinder(G, H).are_isomorphic());

	G = std::move(square_grid(3, 8));
	H = std::move(square_grid(3, 8));
	randomize_vertices(H);
	assert(IsomorphismFinder(G, H).are_isomorphic());
	H = std::move(square_grid(3, 8));
	deleteE(H, Location(0, 3));
	deleteE(H, Location(1, 4));
	addE(H, Location(0, 4));
	addE(H, Location(1, 3));
	H = std::move(square_grid(6, 4));
	randomize_vertices(H);
	assert(!IsomorphismFinder(G, H).are_isomorphic());

	G = std::move(square_grid(3, 40));
	H = std::move(square_grid(3, 40));
	randomize_vertices(H);
	assert(IsomorphismFinder(G, H).are_isomorphic());
	H = std::move(square_grid(4, 30));
	randomize_vertices(H);
	assert(!IsomorphismFinder(G, H).are_isomorphic());

	G = std::move(read_graph6_line(":MkGlWtP?tCvSmXs^"));
	H = std::move(read_graph6_line(":MiIkYf_YX{USmaT~"));
	assert(are_isomorphic(G, H));
	H = std::move(read_graph6_line(":MkUkXDP?tBRMiyD~"));
	assert(are_isomorphic(G, H));

	std::string baseDir = "../../../resources/graphs/";
	std::vector<std::string> files = {
			internal::stored_snark_path(4, 5, 18),
			internal::stored_snark_path(4, 5, 20),
			internal::stored_cubic_path(1, 1, 6),
			internal::stored_cubic_path(1, 1, 8)
			};
	for (auto file : files) {
		auto graphs = read_graph6_file(baseDir + file).graphs();
		for (unsigned i = 0; i < graphs.size(); ++i) {
			randomize_vertices(graphs[i]);
			for (unsigned j = 0; j < graphs.size(); ++j) {
				assert(IsomorphismFinder(graphs[i], graphs[j]).are_isomorphic() == (i == j));
			}
		}
	}

/*
	FIXME too long for a unit test

	std::string vertex_transitive_order1000 = R"(:~?Ng_CKVk{@Rfzxkwh~XkJ_xFU_Y@U@wmd{[edIkQfHxU~rPKw[]FWyUWOV?X@do}wW}OlDNGdHajZHg_XZ?kl`gwObZXpTU]LLFbqDGzA|^VPGsiUa_`TQ{\RPOlSRBDP^PyvgZcInZsRjvdow@l[NiKRw]daJl@_HkiRsiT`k]o}[O_DpC@n]yKbZvW~\sAoLqLGBck{he`DPdBbGH[a[CMIAahhfB`UCDHDjW`@yRBsLks^RDfXasuQJ`AQdsGNJ?GHk]B}Jnf{YOz}Qu@lMbKvq@hAR{sVN\^FEDJSAJPb[dTUkZ_DTI\xH@d{TqzOCbgWHtc~gCKZXhAkLHjW|AYnJvEpsnHCen`{HICybVipcHQhmObnHBFCy_uZUtjhSnKiWrD{LjD^plccOiO`]XIa[VnG\`jDwAhST~^huKfAjJ@c{lYH`{xsOMC]]MsQofVPXD]iYSoORZqgZNI]xqISsv{]Rytuzrg~a|NtW[pj^hWCQjTJK}BhyBqSFM\a^b`hRHH[RmXeJqXKG?ZhJozNRx~E?ewZei}_DeUJQMgj@JJjsGAUJKWCEMzZpNCySAvLCr}gt`_ieCr[LGNSfg[`K{sTCRSBK^i^|Lc_Q\V^D_YsoSmncfgjhc[v?ur]@kENYjPFoQsaNNTfUBtDMHgU\ltSBcxl]fOEM~MIFnTTi[|uliFDYX\ZS{JEQRCsr|KnNKYC|PGaze|oQlxyCdR\O_oEIhSWljjZhN{UCJR?s\Zd[LrkWapkK`[_|CkhR^GKiscu`WXPm|nv\Ot_}]IElYpifhrdDVGOxNxP_dA^wbdNXFj\Zdpbxjy_SAPlTFpGS`DdYUCQaGFBOVl[TJ{_tGJYcgWXvLIdjKWamR}XAzE?smRzM\A[aDFMhe?Xx]{SsWJ\hgQaeqKe\[a_Ea]dqdHeMTuYyfRJaFhpKP{m}g@TDd|Z\zJlHLIXohQyetRijlKDtbMOgQhjSNjOUG|?{RPxgmCzbO]hAKDO|jIikCyu{ckijMXrHFDKZ`CIn`]qItI]eqt`BEWpr[MIq@ZjKLG`e@eoMIae`lzT?cAH|jWvih`u[LQgsot@nSJAeg{`m_UTYmISIzWPGMmydMqTVsUyjSJJyHMua[eHYDPrLtvNapUWl_dYzIbILrzu_k]KWcgRMgH_Haf?WJWsh[hDutIfkd}uDdc_jzbqZEE`UEHpYFmYUr@mG{WQrhdyoI@uPpDJlNWbp[zGgK[lYKNHNjRDvLtJhR{DqXdcceUyPxeeKoaHC`Ytx_{t^GeT@lpTGTEUzS[nA_pkdhUzBDZsF_~QpYGSfLIPbXLslQmGRQ{cUDZTHpL[gZBCvDufOe|}fQRtkoTm_Hkc@jj|mrbImtz|C}iOUGObR`CEO~AOfx\gfxYk}pYENWDE\QegwiwwxI?SC?MAEnKITu\YrJkAnQ`ygYqwwdqK^sMPhvSDVnI~^GOppZyDd_WFloCHQatZ]HBGI@Vz|dinymACIHwxDmvKnE\X_vTsNPbAgkAhKAi]kU[J@?hCHZ?aGSMrBU[Eijngx|umCUfwRCJG|F{P[]jvkkeph|LRomUwGv`wEF@KtKMYrehZVHNLZkadQrKm`uPcEueNcRHvkWf\dacQKDoVU{Z~D[`QgXrIRf}ldgmICaK|pcgfE`hZ]PrXdoPy|GSGSdN[eutaXvfSFp{Mf\dTTYO{V@UleS?CLIaoYxQnqYv_wNqOijRuQo[~^g{\}FYLd|{i[JFtCIH[hcifddW|{kqpp}Mfhxi[YcXj}a]TmCEVr|t^Rj`PKDMB]WgQzKshH{XXHqUTK@TY~KtKVd|escAAWTf]ONBh@bhjCHg`wdL]]\?`XuGfYFbJc{cJm\@FayaTugpeFA|JNohIHUlMHSu]TgUFn`d?bAxrHZzglP~G@[rJ\EFMw`j]EHz|TSKPb`hbjtlIbFqd?CBCyc^SVRPB@[azXbJa_sRTu[beNk}s}WqkEgcnKAcEKYV{[@DENh]tfeZTfCv`TwuUYFRhmrfJjixH]b[uMP__\EBGlI^HfVrCrstN}VSI[|Y|ykJqoRGeu`ILPXT?xU[xk@a~Rl`geQSxzACM@hr|Mg|JCJvY{HWRP?P]b[|eC^OxiBM^`VM}Z]PE|dE[d@TcQcmqUG{BLzWGi]PWSkWgkOi[l]hNMebTzKRSWoi[LolXcvHls|~SBokMZm[ibn[zInWZFTFCtrqZm?Vn@JctYiGrR~edot]bQQMMAoV^Ic||_WvyN[^c^EEoCDMZEWXeSz{BCroSwSmtim^isfMmk[rsjM_?btWThblLKw[l\jV]i{PWBYXa~}~GXBPYRNgucjtAn?woNEDcjsQgeqM|MvZcGknFbgJX_KOr\V_XerabAQem[cH{iBTJiJkQoZdqcFg|FdwzXxQlGqZJcm^]pr?C\_hXP\jKnEDAdZ^m{pUmHyES]\beM]^AaejgEmf_?nXmk{RAIpbeKwbS@QIjjyIJry]KUwSUQkkQ]Yrw?_pO~QsxLISEtdS]qZiOPDU|SNrZ_x{qeWPXCyQu`xDuS\YUV`njhUMvwDW]pJPJJWXhvn{ntd{@rAj]zO?oTB[W|BQIDNM]Pmea}}]TTtMRLjzGTRTv?E~[doW{qffXSdktwKAfCkDwLCC]sSLChZnX@sgo@Un{]itdlOj`eevtotnrhga_OS\h\^C`HP{MMXr[yFA}IGBvDvtAQPHF_aI?l[tT\mshmdlUSOXLUKL\A?wTbT{U`Ulx\IiVdrpxH[ozTbLUhukNEtvrhAL{]_@`M@dB[xIkxa{Vdgs@`tl`_smTsE@NSLdZLyhtK`Bx[c\dAKzceP]uENhGxMj^]EZ?|gUq{Yw{CNJkmpIKPFtDzFUxiL^RuWL_yAvztYn]{y{K_FZ?uSHpiMU_ScYvIcUrx}T~{`g\DKyKm}Wtxrd?CYM~t`PscZNWBFUnWddgft?eewyxCMRpL~pStPnhPjmmUuM~cY^@Vvv_J^_MufzTlhDVR_UunrLZshNdCKt[\\Z^jsJi]L)";
	G = std::move(read_graph6_line(vertex_transitive_order1000));
	H = std::move(read_graph6_line(vertex_transitive_order1000));
	randomize_vertices(H);
	assert(IsomorphismFinder(G, H).are_isomorphic());
*/

	return 0;
}
