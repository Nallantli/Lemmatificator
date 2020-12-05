#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <map>
#include <algorithm>

#include "Search.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#define colorASCII(c) "\033[" + std::to_string(c) + "m"
#define CTEXT(s, c) colorASCII(c) << s << colorASCII(0)

Node::Node(const NounLemma &nl, const NounQuery &nq) : type(NOUN), nounQuery(nq), nounLemma(nl)
{}

Node::Node(const AdjLemma &al, const AdjQuery &aq) : type(ADJECTIVE), adjQuery(aq), adjLemma(al)
{}

Node::Node(const VerbLemma &vl, const VerbQuery &vq) : type(VERB), verbQuery(vq), verbLemma(vl)
{}

enum TextColor
{
	BLACK_TEXT = 30,
	RED_TEXT = 31,
	GREEN_TEXT = 32,
	YELLOW_TEXT = 33,
	BLUE_TEXT = 34,
	MAGENTA_TEXT = 35,
	CYAN_TEXT = 36,
	WHITE_TEXT = 37,
	BRIGHT_BLACK_TEXT = 90,
	BRIGHT_RED_TEXT = 91,
	BRIGHT_GREEN_TEXT = 92,
	BRIGHT_YELLOW_TEXT = 93,
	BRIGHT_BLUE_TEXT = 94,
	BRIGHT_MAGENTA_TEXT = 95,
	BRIGHT_CYAN_TEXT = 96,
	BRIGHT_WHITE_TEXT = 97,

	RESET_TEXT = 0
};

const std::string parseGrapheme(const char &g)
{
	switch (g) {
		case 'a':
			return "a";
		case 'A':
			return "ā";
		case 'e':
			return "e";
		case 'E':
			return "ē";
		case 'i':
			return "i";
		case 'I':
			return "ī";
		case 'o':
			return "o";
		case 'O':
			return "ō";
		case 'u':
			return "u";
		case 'U':
			return "ū";
		case 'y':
			return "y";
		case 'Y':
			return "ȳ";
		case '~':
			return "~";

		default:
			return std::string(1, g);
	}
}

const std::string parseSeries(const series_t &l)
{
	if (l == "*")
		return "*";
	std::string s = "";
	for (auto &c : l)
		s += parseGrapheme(c);
	return s;
}

enum CPerson
{
	_1SG,
	_2SG,
	_3SG,
	_1PL,
	_2PL,
	_3PL
};

const std::string canonicalForm(const AdjLemma &);
const std::string canonicalForm(const NounLemma &);
const std::string canonicalForm(const VerbLemma &);

const series_t getPersonConj1(const CPerson &person)
{
	switch (person) {
		case _2SG:
			return "s";
		case _3SG:
			return "t";
		case _1PL:
			return "mus";
		case _2PL:
			return "tis";
		case _3PL:
			return "nt";
	}
	return "<error>";
}

const series_t getPersonConj2(const CPerson &person)
{
	switch (person) {
		case _2SG:
			return "stI";
		case _3SG:
			return "t";
		case _1PL:
			return "mus";
		case _2PL:
			return "stis";
		case _3PL:
			return "runt";
	}
	return "<error>";
}

const series_t getPersonConj3(const CPerson &person)
{
	switch (person) {
		case _2SG:
			return "ris";
		case _3SG:
			return "tur";
		case _1PL:
			return "mur";
		case _2PL:
			return "minI";
		case _3PL:
			return "ntur";
	}
	return "<error>";
}

const series_t getPersonConj4(const CPerson &person, const bool &future)
{
	switch (person) {
		case _2SG:
		case _3SG:
			return future ? "tO" : "";
		case _2PL:
			return future ? "tOte" : "te";
		case _3PL:
			return "ntO";
	}
	return "<error>";
}

const series_t getPersonConj5(const CPerson &person, const bool &future)
{
	switch (person) {
		case _2SG:
		case _3SG:
			return future ? "tor" : "re";
		case _2PL:
			return "minI";
		case _3PL:
			return "ntor";
	}
	return "<error>";
}

const series_t conjugate(const VerbLemma &vl, const ConjugationSchema &csch)
{
	series_t ret;
	CPerson person;
	bool future;
	bool simple = true;
	switch (csch) {
		case INF_ACT_PRE:
			ret = vl.active_simple.inf;
			break;
		case INF_ACT_PRF:
			ret = vl.active_perfect.inf;
			simple = false;
			break;
		case INF_PAS_PRE:
			ret = vl.passive_simple.inf;
			break;

		case IMP_ACT_PRE_2SG:
			ret = vl.active_simple.imp1;
			person = _2SG;
			future = false;
			break;
		case IMP_ACT_PRE_2PL:
			ret = vl.active_simple.imp2;
			person = _2PL;
			future = false;
			break;
		case IMP_ACT_FUT_2SG:
			ret = vl.active_simple.imp2;
			person = _2SG;
			future = true;
			break;
		case IMP_ACT_FUT_3SG:
			ret = vl.active_simple.imp2;
			person = _3SG;
			future = true;
			break;
		case IMP_ACT_FUT_2PL:
			ret = vl.active_simple.imp2;
			person = _2PL;
			future = true;
			break;
		case IMP_ACT_FUT_3PL:
			ret = vl.active_simple.imp3;
			person = _3PL;
			future = true;
			break;

		case IMP_PAS_PRE_2SG:
			ret = vl.passive_simple.imp1;
			person = _2SG;
			future = false;
			break;
		case IMP_PAS_PRE_2PL:
			ret = vl.passive_simple.imp2;
			person = _2PL;
			future = false;
			break;
		case IMP_PAS_FUT_2SG:
			ret = vl.passive_simple.imp2;
			person = _2SG;
			future = true;
			break;
		case IMP_PAS_FUT_3SG:
			ret = vl.passive_simple.imp2;
			person = _3SG;
			future = true;
			break;
		case IMP_PAS_FUT_3PL:
			ret = vl.passive_simple.imp3;
			person = _3PL;
			future = true;
			break;

		case IND_ACT_SIM_PRE_1SG:
			ret = vl.active_simple.ind_pres._1sg;
			person = _1SG;
			break;
		case IND_ACT_SIM_PRE_2SG:
			ret = vl.active_simple.ind_pres._2sg;
			person = _2SG;
			break;
		case IND_ACT_SIM_PRE_3SG:
			ret = vl.active_simple.ind_pres._3sg;
			person = _3SG;
			break;
		case IND_ACT_SIM_PRE_1PL:
			ret = vl.active_simple.ind_pres._1pl;
			person = _1PL;
			break;
		case IND_ACT_SIM_PRE_2PL:
			ret = vl.active_simple.ind_pres._2pl;
			person = _2PL;
			break;
		case IND_ACT_SIM_PRE_3PL:
			ret = vl.active_simple.ind_pres._3pl;
			person = _3PL;
			break;

		case IND_ACT_SIM_IMP_1SG:
			ret = vl.active_simple.ind_impf._1sg;
			person = _1SG;
			break;
		case IND_ACT_SIM_IMP_2SG:
			ret = vl.active_simple.ind_impf._2sg;
			person = _2SG;
			break;
		case IND_ACT_SIM_IMP_3SG:
			ret = vl.active_simple.ind_impf._3sg;
			person = _3SG;
			break;
		case IND_ACT_SIM_IMP_1PL:
			ret = vl.active_simple.ind_impf._1pl;
			person = _1PL;
			break;
		case IND_ACT_SIM_IMP_2PL:
			ret = vl.active_simple.ind_impf._2pl;
			person = _2PL;
			break;
		case IND_ACT_SIM_IMP_3PL:
			ret = vl.active_simple.ind_impf._3pl;
			person = _3PL;
			break;

		case IND_ACT_SIM_FUT_1SG:
			ret = vl.active_simple.ind_fut._1sg;
			person = _1SG;
			break;
		case IND_ACT_SIM_FUT_2SG:
			ret = vl.active_simple.ind_fut._2sg;
			person = _2SG;
			break;
		case IND_ACT_SIM_FUT_3SG:
			ret = vl.active_simple.ind_fut._3sg;
			person = _3SG;
			break;
		case IND_ACT_SIM_FUT_1PL:
			ret = vl.active_simple.ind_fut._1pl;
			person = _1PL;
			break;
		case IND_ACT_SIM_FUT_2PL:
			ret = vl.active_simple.ind_fut._2pl;
			person = _2PL;
			break;
		case IND_ACT_SIM_FUT_3PL:
			ret = vl.active_simple.ind_fut._3pl;
			person = _3PL;
			break;

		case IND_ACT_PRF_PRE_1SG:
			ret = vl.active_perfect.ind_pres._1sg;
			person = _1SG;
			simple = false;
			break;
		case IND_ACT_PRF_PRE_2SG:
			ret = vl.active_perfect.ind_pres._2sg;
			person = _2SG;
			break;
		case IND_ACT_PRF_PRE_3SG:
			ret = vl.active_perfect.ind_pres._3sg;
			person = _3SG;
			simple = false;
			break;
		case IND_ACT_PRF_PRE_1PL:
			ret = vl.active_perfect.ind_pres._1pl;
			person = _1PL;
			simple = false;
			break;
		case IND_ACT_PRF_PRE_2PL:
			ret = vl.active_perfect.ind_pres._2pl;
			person = _2PL;
			simple = false;
			break;
		case IND_ACT_PRF_PRE_3PL:
			ret = vl.active_perfect.ind_pres._3pl;
			person = _3PL;
			simple = false;
			break;

		case IND_ACT_PRF_IMP_1SG:
			ret = vl.active_perfect.ind_impf._1sg;
			person = _1SG;
			simple = false;
			break;
		case IND_ACT_PRF_IMP_2SG:
			ret = vl.active_perfect.ind_impf._2sg;
			person = _2SG;
			simple = false;
			break;
		case IND_ACT_PRF_IMP_3SG:
			ret = vl.active_perfect.ind_impf._3sg;
			person = _3SG;
			simple = false;
			break;
		case IND_ACT_PRF_IMP_1PL:
			ret = vl.active_perfect.ind_impf._1pl;
			person = _1PL;
			simple = false;
			break;
		case IND_ACT_PRF_IMP_2PL:
			ret = vl.active_perfect.ind_impf._2pl;
			person = _2PL;
			simple = false;
			break;
		case IND_ACT_PRF_IMP_3PL:
			ret = vl.active_perfect.ind_impf._3pl;
			person = _3PL;
			simple = false;
			break;

		case IND_ACT_PRF_FUT_1SG:
			ret = vl.active_perfect.ind_fut._1sg;
			person = _1SG;
			simple = false;
			break;
		case IND_ACT_PRF_FUT_2SG:
			ret = vl.active_perfect.ind_fut._2sg;
			person = _2SG;
			simple = false;
			break;
		case IND_ACT_PRF_FUT_3SG:
			ret = vl.active_perfect.ind_fut._3sg;
			person = _3SG;
			simple = false;
			break;
		case IND_ACT_PRF_FUT_1PL:
			ret = vl.active_perfect.ind_fut._1pl;
			person = _1PL;
			simple = false;
			break;
		case IND_ACT_PRF_FUT_2PL:
			ret = vl.active_perfect.ind_fut._2pl;
			person = _2PL;
			simple = false;
			break;
		case IND_ACT_PRF_FUT_3PL:
			ret = vl.active_perfect.ind_fut._3pl;
			person = _3PL;
			simple = false;
			break;

		case IND_PAS_SIM_PRE_1SG:
			ret = vl.passive_simple.ind_pres._1sg;
			person = _1SG;
			break;
		case IND_PAS_SIM_PRE_2SG:
			ret = vl.passive_simple.ind_pres._2sg;
			person = _2SG;
			break;
		case IND_PAS_SIM_PRE_3SG:
			ret = vl.passive_simple.ind_pres._3sg;
			person = _3SG;
			break;
		case IND_PAS_SIM_PRE_1PL:
			ret = vl.passive_simple.ind_pres._1pl;
			person = _1PL;
			break;
		case IND_PAS_SIM_PRE_2PL:
			ret = vl.passive_simple.ind_pres._2pl;
			person = _2PL;
			break;
		case IND_PAS_SIM_PRE_3PL:
			ret = vl.passive_simple.ind_pres._3pl;
			person = _3PL;
			break;

		case IND_PAS_SIM_IMP_1SG:
			ret = vl.passive_simple.ind_impf._1sg;
			person = _1SG;
			break;
		case IND_PAS_SIM_IMP_2SG:
			ret = vl.passive_simple.ind_impf._2sg;
			person = _2SG;
			break;
		case IND_PAS_SIM_IMP_3SG:
			ret = vl.passive_simple.ind_impf._3sg;
			person = _3SG;
			break;
		case IND_PAS_SIM_IMP_1PL:
			ret = vl.passive_simple.ind_impf._1pl;
			person = _1PL;
			break;
		case IND_PAS_SIM_IMP_2PL:
			ret = vl.passive_simple.ind_impf._2pl;
			person = _2PL;
			break;
		case IND_PAS_SIM_IMP_3PL:
			ret = vl.passive_simple.ind_impf._3pl;
			person = _3PL;
			break;

		case IND_PAS_SIM_FUT_1SG:
			ret = vl.passive_simple.ind_fut._1sg;
			person = _1SG;
			break;
		case IND_PAS_SIM_FUT_2SG:
			ret = vl.passive_simple.ind_fut._2sg;
			person = _2SG;
			break;
		case IND_PAS_SIM_FUT_3SG:
			ret = vl.passive_simple.ind_fut._3sg;
			person = _3SG;
			break;
		case IND_PAS_SIM_FUT_1PL:
			ret = vl.passive_simple.ind_fut._1pl;
			person = _1PL;
			break;
		case IND_PAS_SIM_FUT_2PL:
			ret = vl.passive_simple.ind_fut._2pl;
			person = _2PL;
			break;
		case IND_PAS_SIM_FUT_3PL:
			ret = vl.passive_simple.ind_fut._3pl;
			person = _3PL;
			break;

		case SUB_ACT_SIM_PRE_1SG:
			ret = vl.active_simple.sub_pres._1sg;
			person = _1SG;
			break;
		case SUB_ACT_SIM_PRE_2SG:
			ret = vl.active_simple.sub_pres._2sg;
			person = _2SG;
			break;
		case SUB_ACT_SIM_PRE_3SG:
			ret = vl.active_simple.sub_pres._3sg;
			person = _3SG;
			break;
		case SUB_ACT_SIM_PRE_1PL:
			ret = vl.active_simple.sub_pres._1pl;
			person = _1PL;
			break;
		case SUB_ACT_SIM_PRE_2PL:
			ret = vl.active_simple.sub_pres._2pl;
			person = _2PL;
			break;
		case SUB_ACT_SIM_PRE_3PL:
			ret = vl.active_simple.sub_pres._3pl;
			person = _3PL;
			break;

		case SUB_ACT_SIM_IMP_1SG:
			ret = vl.active_simple.sub_impf._1sg;
			person = _1SG;
			break;
		case SUB_ACT_SIM_IMP_2SG:
			ret = vl.active_simple.sub_impf._2sg;
			person = _2SG;
			break;
		case SUB_ACT_SIM_IMP_3SG:
			ret = vl.active_simple.sub_impf._3sg;
			person = _3SG;
			break;
		case SUB_ACT_SIM_IMP_1PL:
			ret = vl.active_simple.sub_impf._1pl;
			person = _1PL;
			break;
		case SUB_ACT_SIM_IMP_2PL:
			ret = vl.active_simple.sub_impf._2pl;
			person = _2PL;
			break;
		case SUB_ACT_SIM_IMP_3PL:
			ret = vl.active_simple.sub_impf._3pl;
			person = _3PL;
			break;

		case SUB_ACT_PRF_PRE_1SG:
			ret = vl.active_perfect.sub_pres._1sg;
			person = _1SG;
			simple = false;
			break;
		case SUB_ACT_PRF_PRE_2SG:
			ret = vl.active_perfect.sub_pres._2sg;
			person = _2SG;
			simple = false;
			break;
		case SUB_ACT_PRF_PRE_3SG:
			ret = vl.active_perfect.sub_pres._3sg;
			person = _3SG;
			simple = false;
			break;
		case SUB_ACT_PRF_PRE_1PL:
			ret = vl.active_perfect.sub_pres._1pl;
			person = _1PL;
			simple = false;
			break;
		case SUB_ACT_PRF_PRE_2PL:
			ret = vl.active_perfect.sub_pres._2pl;
			person = _2PL;
			simple = false;
			break;
		case SUB_ACT_PRF_PRE_3PL:
			ret = vl.active_perfect.sub_pres._3pl;
			person = _3PL;
			simple = false;
			break;

		case SUB_ACT_PRF_IMP_1SG:
			ret = vl.active_perfect.sub_impf._1sg;
			person = _1SG;
			simple = false;
			break;
		case SUB_ACT_PRF_IMP_2SG:
			ret = vl.active_perfect.sub_impf._2sg;
			person = _2SG;
			simple = false;
			break;
		case SUB_ACT_PRF_IMP_3SG:
			ret = vl.active_perfect.sub_impf._3sg;
			person = _3SG;
			simple = false;
			break;
		case SUB_ACT_PRF_IMP_1PL:
			ret = vl.active_perfect.sub_impf._1pl;
			person = _1PL;
			simple = false;
			break;
		case SUB_ACT_PRF_IMP_2PL:
			ret = vl.active_perfect.sub_impf._2pl;
			person = _2PL;
			simple = false;
			break;
		case SUB_ACT_PRF_IMP_3PL:
			ret = vl.active_perfect.sub_impf._3pl;
			person = _3PL;
			simple = false;
			break;

		case SUB_PAS_SIM_PRE_1SG:
			ret = vl.passive_simple.sub_pres._1sg;
			person = _1SG;
			break;
		case SUB_PAS_SIM_PRE_2SG:
			ret = vl.passive_simple.sub_pres._2sg;
			person = _2SG;
			break;
		case SUB_PAS_SIM_PRE_3SG:
			ret = vl.passive_simple.sub_pres._3sg;
			person = _3SG;
			break;
		case SUB_PAS_SIM_PRE_1PL:
			ret = vl.passive_simple.sub_pres._1pl;
			person = _1PL;
			break;
		case SUB_PAS_SIM_PRE_2PL:
			ret = vl.passive_simple.sub_pres._2pl;
			person = _2PL;
			break;
		case SUB_PAS_SIM_PRE_3PL:
			ret = vl.passive_simple.sub_pres._3pl;
			person = _3PL;
			break;

		case SUB_PAS_SIM_IMP_1SG:
			ret = vl.passive_simple.sub_impf._1sg;
			person = _1SG;
			break;
		case SUB_PAS_SIM_IMP_2SG:
			ret = vl.passive_simple.sub_impf._2sg;
			person = _2SG;
			break;
		case SUB_PAS_SIM_IMP_3SG:
			ret = vl.passive_simple.sub_impf._3sg;
			person = _3SG;
			break;
		case SUB_PAS_SIM_IMP_1PL:
			ret = vl.passive_simple.sub_impf._1pl;
			person = _1PL;
			break;
		case SUB_PAS_SIM_IMP_2PL:
			ret = vl.passive_simple.sub_impf._2pl;
			person = _2PL;
			break;
		case SUB_PAS_SIM_IMP_3PL:
			ret = vl.passive_simple.sub_impf._3pl;
			person = _3PL;
			break;
	}
	series_t seq;
	for (auto &c : ret) {
		switch (c) {
			case '!':
				seq += (simple ? vl.sim_stem : vl.prf_stem);
				break;
			case '+':
				seq += vl.extra_stem;
				break;
			case '@':
				seq += getPersonConj1(person);
				break;
			case '#':
				seq += getPersonConj2(person);
				break;
			case '$':
				seq += getPersonConj3(person);
				break;
			case '%':
				seq += getPersonConj4(person, future);
				break;
			case '^':
				seq += getPersonConj5(person, future);
				break;
			case '*':
				return "*";
			default:
				seq += c;
				break;
		}
	}
	return seq;
}

const series_t decline(const NounLemma &nl, const Inflection &inflection)
{
	series_t suffix;
	switch (inflection) {
		case NOM_SG:
			suffix = nl.decl.nom.sg;
			break;
		case NOM_PL:
			suffix = nl.decl.nom.pl;
			break;
		case GEN_SG:
			suffix = nl.decl.gen.sg;
			break;
		case GEN_PL:
			suffix = nl.decl.gen.pl;
			break;
		case DAT_SG:
			suffix = nl.decl.dat.sg;
			break;
		case DAT_PL:
			suffix = nl.decl.dat.pl;
			break;
		case ACC_SG:
			suffix = nl.decl.acc.sg;
			break;
		case ACC_PL:
			suffix = nl.decl.acc.pl;
			break;
		case ABL_SG:
			suffix = nl.decl.abl.sg;
			break;
		case ABL_PL:
			suffix = nl.decl.abl.pl;
			break;
		case VOC_SG:
			suffix = nl.decl.voc.sg;
			break;
		case VOC_PL:
			suffix = nl.decl.voc.pl;
			break;
		case LOC_SG:
			suffix = nl.decl.loc.sg;
			break;
		case LOC_PL:
			suffix = nl.decl.loc.pl;
			break;
	}
	series_t ret = "";
	for (auto &c : suffix) {
		switch (c) {
			case '*':
				return "*";
			case '@':
				return nl.lemma;
			case '$':
				ret += nl.stem;
				break;
			default:
				ret += c;
				break;
		}
	}
	return ret;
}

const series_t decline(const AdjLemma &al, const Inflection &inflection, const Gender &g)
{
	series_t suffix;
	series_t lemma;
	Declension d;
	switch (g) {
		case G_MAS:
			d = al.mas;
			lemma = al.mlemma;
			break;
		case G_FEM:
			d = al.fem;
			lemma = al.flemma;
			break;
		case G_NEU:
			d = al.neu;
			lemma = al.nlemma;
			break;
	}
	switch (inflection) {
		case NOM_SG:
			suffix = d.nom.sg;
			break;
		case NOM_PL:
			suffix = d.nom.pl;
			break;
		case GEN_SG:
			suffix = d.gen.sg;
			break;
		case GEN_PL:
			suffix = d.gen.pl;
			break;
		case DAT_SG:
			suffix = d.dat.sg;
			break;
		case DAT_PL:
			suffix = d.dat.pl;
			break;
		case ACC_SG:
			suffix = d.acc.sg;
			break;
		case ACC_PL:
			suffix = d.acc.pl;
			break;
		case ABL_SG:
			suffix = d.abl.sg;
			break;
		case ABL_PL:
			suffix = d.abl.pl;
			break;
		case VOC_SG:
			suffix = d.voc.sg;
			break;
		case VOC_PL:
			suffix = d.voc.pl;
			break;
		case LOC_SG:
			suffix = d.loc.sg;
			break;
		case LOC_PL:
			suffix = d.loc.pl;
			break;
	}
	if (al.suffix != "*")
		suffix += al.suffix;
	series_t ret = "";
	for (auto &c : suffix) {
		switch (c) {
			case '*':
				return "*";
			case '@':
				return lemma;
			case '$':
				ret += al.stem;
				break;
			default:
				ret += c;
				break;
		}
	}
	return ret;
}

const std::string declensionName(const Inflection &inflection)
{
	series_t suffix;
	switch (inflection) {
		case NOM_SG:
			return "nominative singular";
		case NOM_PL:
			return "nominative plural";
		case GEN_SG:
			return "genitive singular";
		case GEN_PL:
			return "genitive plural";
		case DAT_SG:
			return "dative singular";
		case DAT_PL:
			return "dative plural";
		case ACC_SG:
			return "accusative singular";
		case ACC_PL:
			return "accusative plural";
		case ABL_SG:
			return "ablative singular";
		case ABL_PL:
			return "ablative plural";
		case VOC_SG:
			return "vocative singular";
		case VOC_PL:
			return "vocative plural";
		case LOC_SG:
			return "locative singular";
		case LOC_PL:
			return "locative plural";
		default:
			return "<error>";
	}
}

const std::string genderName(const Gender &gender)
{
	switch (gender) {
		case G_MAS:
			return "masculine";
		case G_FEM:
			return "feminine";
		case G_NEU:
			return "neuter";
		default:
			return "<error>";
	}
}

std::unordered_map<std::string, Declension> DECLS;
std::unordered_map<std::string, Conjugation> CONJ;

const Declension readDeclension(const std::string &filename)
{
	if (DECLS.find(filename) != DECLS.end())
		return DECLS[filename];
	std::ifstream file;
	file.open(std::filesystem::current_path() / "data" / "decl" / filename);
	if (!file.is_open()) {
		std::cerr << "Cannot open " << filename << "\n";
	}
	std::vector<series_t> decl;
	std::string line;
	while (std::getline(file, line)) {
		decl.push_back(line);
	}
	file.close();

	DECLS[filename] = {
		decl[0],
		decl[1],
		decl[2],
		decl[3],
		decl[4],
		decl[5],
		decl[6],
		decl[7],
		decl[8],
		decl[9],
		decl[10],
		decl[11],
		decl[12],
		decl[13],
		decl[14]
	};

	return DECLS[filename];
}

const Conjugation readConjugation(const std::string &filename)
{
	if (filename == "*")
		return Conjugation();
	if (CONJ.find(filename) != CONJ.end())
		return CONJ[filename];
	std::ifstream file;
	file.open(std::filesystem::current_path() / "data" / "conj" / filename);
	if (!file.is_open()) {
		std::cerr << "Cannot open " << filename << "\n";
	}
	std::vector<series_t> conj;
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty())
			continue;
		conj.push_back(line);
	}
	file.close();

	CONJ[filename] = {
		// name
		conj[0],
		// inf
		conj[1],
		//imp
		conj[2],
		conj[3],
		conj[4],
		// ind pres
		{
			conj[5],
			conj[6],
			conj[7],
			conj[8],
			conj[9],
			conj[10],
		},
		// ind impf
		{
			conj[11],
			conj[12],
			conj[13],
			conj[14],
			conj[15],
			conj[16]
		},
		// ind fut
		{
			conj[17],
			conj[18],
			conj[19],
			conj[20],
			conj[21],
			conj[22]
		},
		// sub pres
		{
			conj[23],
			conj[24],
			conj[25],
			conj[26],
			conj[27],
			conj[28]
		},
		// sub impf
		{
			conj[29],
			conj[30],
			conj[31],
			conj[32],
			conj[33],
			conj[34]
		}
	};

	return CONJ[filename];
}

const std::vector<std::string> parseTabbedLine(const std::string &line)
{
	std::vector<std::string> contents = { "" };

	for (auto &c : line) {
		if (c == '\t') {
			if (contents.back() == "")
				continue;
			contents.push_back("");
		} else {
			contents.back().push_back(c);
		}
	}
	return contents;
}

void registerNounLemma(const NounLemma &lemma, SearchMap *search_map)
{
	for (int i = 0; i < 14; i++) {
		auto current = search_map;
		auto d = decline(lemma, (Inflection)i);
		if (d != "*") {
			for (auto &c : d) {
				current = &current->next[c];
			}
			Node n(
				lemma,
				{ (Inflection)i }
			);
			current->lemmas.push_back(n);
		}
	}
	/*auto current = search_map;
	for (auto &c : lemma.lemma) {
		current = &current->next[c];
	}
	current->noun_lemmas.push_back(lemma);

	if (lemma.lemma != lemma.stem) {
		current = search_map;
		for (auto &c : lemma.stem) {
			current = &current->next[c];
		}
		current->noun_lemmas.push_back(lemma);
	}*/
}

void registerAdjLemma(const AdjLemma &lemma, SearchMap *search_map)
{
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 14; i++) {
			auto current = search_map;
			auto d = decline(lemma, (Inflection)i, (Gender)j);
			if (d != "*") {
				for (auto &c : d) {
					current = &current->next[c];
				}
				Node n(
					lemma,
					{ (Inflection)i, (Gender)j }
				);
				current->lemmas.push_back(n);
			}
		}
	}
	/*auto current = search_map;
	for (auto &c : lemma.mlemma) {
		current = &current->next[c];
	}
	current->adj_lemmas.push_back(lemma);

	if (lemma.flemma != lemma.mlemma) {
		current = search_map;
		for (auto &c : lemma.flemma) {
			current = &current->next[c];
		}
		current->adj_lemmas.push_back(lemma);
	}

	if (lemma.nlemma != lemma.mlemma && lemma.nlemma != lemma.flemma) {
		current = search_map;
		for (auto &c : lemma.nlemma) {
			current = &current->next[c];
		}
		current->adj_lemmas.push_back(lemma);
	}

	if (lemma.stem != lemma.mlemma && lemma.stem != lemma.flemma && lemma.stem != lemma.nlemma) {
		current = search_map;
		for (auto &c : lemma.stem) {
			current = &current->next[c];
		}
		current->adj_lemmas.push_back(lemma);
	}*/
}

void registerVerbLemma(const VerbLemma &lemma, SearchMap *search_map)
{
	for (int i = 0; i < 104; i++) {
		auto current = search_map;
		auto d = conjugate(lemma, (ConjugationSchema)i);
		if (d != "*") {
			for (auto &c : d) {
				current = &current->next[c];
			}
			Node n(
				lemma,
				{ (ConjugationSchema)i }
			);
			current->lemmas.push_back(n);
		}
	}
	/*auto current = search_map;
	if (lemma.sim_stem == "~") {
		for (int i = 0; i < 104; i++) {
			current = search_map;
			auto d = conjugate(lemma, (ConjugationSchema)i);
			for (auto &c : d) {
				current = &current->next[c];
			}
			current->verb_lemmas.push_back(lemma);
		}
	} else {
		for (auto &c : lemma.sim_stem) {
			current = &current->next[c];
		}
		current->verb_lemmas.push_back(lemma);
	}

	if (lemma.prf_stem != lemma.sim_stem && lemma.prf_stem != "*") {
		current = search_map;
		for (auto &c : lemma.prf_stem) {
			current = &current->next[c];
		}
		current->verb_lemmas.push_back(lemma);
	}

	if (lemma.extra_stem != "*") {
		current = search_map;
		for (auto &c : lemma.extra_stem) {
			current = &current->next[c];
		}
		current->verb_lemmas.push_back(lemma);
	}*/
}

void readNouns(SearchMap *search_map)
{
	std::ifstream file;
	file.open(std::filesystem::current_path() / "data" / "nouns");
	if (!file.is_open()) {
		std::cerr << "Cannot open noun lemmas\n";
	}

	std::string line;
	while (std::getline(file, line)) {
		auto contents = parseTabbedLine(line);

		NounLemma nl = {
			contents[0],
			contents[1],
			contents[2],
			(contents[3] == "M" ? G_MAS : (contents[3] == "N" ? G_NEU : G_FEM)),
			readDeclension(contents[4]),
			contents[5]
		};
		registerNounLemma(nl, search_map);
	}
	file.close();
}

void readAdjs(SearchMap *search_map)
{
	std::ifstream file;
	file.open(std::filesystem::current_path() / "data" / "adjs");
	if (!file.is_open()) {
		std::cerr << "Cannot open adj lemmas\n";
	}

	std::string line;
	while (std::getline(file, line)) {
		auto contents = parseTabbedLine(line);

		AdjLemma nl = {
			A_POS,
			contents[0],
			contents[1],
			contents[2],
			contents[3],
			contents[4],
			readDeclension(contents[7]),
			readDeclension(contents[8]),
			readDeclension(contents[9]),
			contents[10]
		};
		registerAdjLemma(nl, search_map);

		if (contents[5] != "*") {
			AdjLemma cal = {
				A_COMP,
				contents[5] + "or",
				contents[5] + "or",
				contents[5] + "us",
				contents[5] + "Or",
				"*",
				readDeclension("L3"),
				readDeclension("L3"),
				readDeclension("L3N"),
				"Comparative of " + canonicalForm(nl)
			};
			registerAdjLemma(cal, search_map);
		}

		if (contents[6] != "*") {
			AdjLemma cal = {
				A_SUPR,
				contents[6] + "us",
				contents[6] + "a",
				contents[6] + "um",
				contents[6],
				"*",
				readDeclension("L2M"),
				readDeclension("L1"),
				readDeclension("L3N"),
				"Superlative of " + canonicalForm(nl)
			};
			registerAdjLemma(cal, search_map);
		}
	}
	file.close();
}

void readVerbs(SearchMap *search_map)
{
	std::ifstream file;
	file.open(std::filesystem::current_path() / "data" / "verbs");
	if (!file.is_open()) {
		std::cerr << "Cannot open verb lemmas\n";
	}

	std::string line;
	while (std::getline(file, line)) {
		auto contents = parseTabbedLine(line);

		VerbLemma vl = {
			contents[0],
			contents[1],
			contents[2],
			contents[3],
			contents[4],
			contents[5],
			contents[6],
			readConjugation(contents[7]),
			readConjugation(contents[8]),
			readConjugation(contents[9]),
			contents[10]
		};
		registerVerbLemma(vl, search_map);

		if (contents[3] != "*") {
			AdjLemma cal = {
				A_POS,
				contents[3] + "us",
				contents[3] + "a",
				contents[3] + "um",
				contents[3],
				"*",
				readDeclension("L2M"),
				readDeclension("L1"),
				readDeclension("L2N"),
				"Perfect passive participle or supine of " + canonicalForm(vl)
			};
			registerAdjLemma(cal, search_map);
		}

		if (contents[4] != "*") {
			AdjLemma cal = {
				A_POS,
				contents[4] + "us",
				contents[4] + "a",
				contents[4] + "um",
				contents[4],
				"*",
				readDeclension("L2M"),
				readDeclension("L1"),
				readDeclension("L2N"),
				"Future passive participle or gerundive of " + canonicalForm(vl)
			};
			registerAdjLemma(cal, search_map);
		}

		if (contents[5] != "*") {
			AdjLemma cal = {
				A_POS,
				contents[5] + "s",
				contents[5] + "s",
				contents[5] + "s",
				contents[5] + "t",
				"*",
				readDeclension("L3I"),
				readDeclension("L3I"),
				readDeclension("L3NIA"),
				"Present active participle of " + canonicalForm(vl)
			};
			registerAdjLemma(cal, search_map);

			AdjLemma scal = {
				A_POS,
				contents[5] + "tissimus",
				contents[5] + "tissima",
				contents[5] + "tissimum",
				contents[5] + "tissim",
				"*",
				readDeclension("L2M"),
				readDeclension("L1"),
				readDeclension("L2N"),
				"Superlative of " + canonicalForm(cal)
			};
			registerAdjLemma(scal, search_map);
		}

		if (contents[6] != "*") {
			AdjLemma cal = {
				A_POS,
				contents[6] + "us",
				contents[6] + "a",
				contents[6] + "um",
				contents[6],
				"*",
				readDeclension("L2M"),
				readDeclension("L1"),
				readDeclension("L2N"),
				"Future active participle of " + canonicalForm(vl)
			};
			registerAdjLemma(cal, search_map);
		}
	}
	file.close();
}

const SearchMap *searchSequence(const series_t &s, const SearchMap *search_map)
{
	auto current = search_map;
	for (auto &c : s) {
		if (current->next.find(c) == current->next.end())
			return NULL;
		current = &current->next.at(c);
	}
	return current;
}

const SearchMap *searchSequenceExact(const series_t &s, const SearchMap *search_map)
{
	auto current = search_map;
	for (auto &c : s) {
		if (current->next.find(c) == current->next.end())
			return NULL;
		current = &current->next.at(c);
	}
	if (!current->lemmas.empty())
		return current;
	return NULL;
}

const std::vector<Node> findLemmaSequence(const series_t &s, const SearchMap *search_map)
{
	std::vector<Node> lemmas;
	auto find = searchSequenceExact(s, search_map);
	if (find != NULL) {
		for (auto &l : find->lemmas) {
			if (std::find(lemmas.begin(), lemmas.end(), l) == lemmas.end())
				lemmas.push_back(l);
		}
	}
	return lemmas;
}

/*const std::vector<std::pair<NounLemma, Inflection>> findNounSequence(const series_t &s, const SearchMap *search_map)
{
	std::vector<std::pair<NounLemma, Inflection>> lemmas;
	for (int j = 0; j < s.length(); j++) {
		auto find = searchSequenceExact(s.substr(0, j + 1), search_map);
		if (find != NULL) {
			for (auto &l : find->noun_lemmas) {
				for (int i = 0; i < 14; i++) {
					auto d = decline(l, (Inflection)i);
					if (d == s && std::find(lemmas.begin(), lemmas.end(), std::pair<NounLemma, Inflection>({ l, (Inflection)i })) == lemmas.end())
						lemmas.push_back({ l, (Inflection)i });
				}
			}
		}
	}
	return lemmas;
}

const std::vector<std::pair<AdjLemma, std::pair<Inflection, Gender>>> findAdjSequence(const series_t &s, const SearchMap *search_map)
{
	std::vector<std::pair<AdjLemma, std::pair<Inflection, Gender>>> lemmas;
	for (int j = 0; j < s.length(); j++) {
		auto find = searchSequenceExact(s.substr(0, j + 1), search_map);
		if (find != NULL) {
			for (auto &l : find->adj_lemmas) {
				for (int k = 0; k < 3; k++) {
					for (int i = 0; i < 14; i++) {
						auto d = decline(l, (Inflection)i, (Gender)k);
						if (d == s && std::find(lemmas.begin(), lemmas.end(), std::pair<AdjLemma, std::pair<Inflection, Gender>>({ l, {(Inflection)i, (Gender)k} })) == lemmas.end())
							lemmas.push_back({ l, {(Inflection)i, (Gender)k} });
					}
				}
			}
		}
	}
	return lemmas;
}

const std::vector<std::pair<VerbLemma, ConjugationSchema>> findVerbSequence(const series_t &s, const SearchMap *search_map)
{
	std::vector<std::pair<VerbLemma, ConjugationSchema>> lemmas;
	for (int j = 0; j < s.length(); j++) {
		auto find = searchSequenceExact(s.substr(0, j + 1), search_map);
		if (find != NULL) {
			for (auto &l : find->verb_lemmas) {
				for (int i = 0; i < 104; i++) {
					auto d = conjugate(l, (ConjugationSchema)i);
					if (d == s && std::find(lemmas.begin(), lemmas.end(), std::pair<VerbLemma, ConjugationSchema>({ l, (ConjugationSchema)i })) == lemmas.end())
						lemmas.push_back({ l, (ConjugationSchema)i });
				}
			}
		}
	}
	return lemmas;
}*/

const std::string canonicalForm(const NounLemma &nl)
{
	if (nl.genov != "*")
		return parseSeries(nl.lemma) + ", " + parseSeries(nl.genov) + " (" + (nl.gender == G_MAS ? "M" : (nl.gender == G_NEU ? "N" : "F")) + ")";
	return parseSeries(nl.lemma) + ", " + parseSeries(decline(nl, GEN_SG)) + " (" + (nl.gender == G_MAS ? "M" : (nl.gender == G_NEU ? "N" : "F")) + ")";
}

const std::string canonicalForm(const AdjLemma &al)
{
	return parseSeries(al.mlemma) + ", " + parseSeries(al.flemma) + ", " + parseSeries(al.nlemma);
}

const std::string canonicalForm(const VerbLemma &vl)
{
	return parseSeries(conjugate(vl, IND_ACT_SIM_PRE_1SG)) + ", " + parseSeries(conjugate(vl, INF_ACT_PRE)) + ", " + parseSeries(conjugate(vl, IND_ACT_PRF_PRE_1SG)) + ", " + parseSeries(vl.sup_stem + "um");
}

const std::string canonicalForm(const Node &n)
{
	switch (n.type) {
		case NOUN:
			return canonicalForm(n.nounLemma);
		case ADJECTIVE:
			return canonicalForm(n.adjLemma);
		case VERB:
			return canonicalForm(n.verbLemma);
		default:
			return "<error>";
	}
}

template<typename T>
const std::vector<T> addToAll(const std::vector<T> &v, const T &item)
{
	std::vector<T> nv;
	for (auto &e : v) {
		nv.push_back(e + item);
	}
	return nv;
}

template<typename T>
const std::vector<T> combine(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> nv = a;
	for (auto &e : b) {
		nv.push_back(e);
	}
	return nv;
}

const std::vector<series_t> generatePossibilities(const std::string &s, const std::vector<series_t> &prev)
{
	if (s.empty())
		return prev;
	switch (s[0]) {
		case 'a': {
			auto nv = generatePossibilities(s.substr(1), addToAll<series_t>(prev, "a"));
			nv = combine(nv, generatePossibilities(s.substr(1), addToAll<series_t>(prev, "A")));
			return nv;
		}
		case 'e': {
			auto nv = generatePossibilities(s.substr(1), addToAll<series_t>(prev, "e"));
			nv = combine(nv, generatePossibilities(s.substr(1), addToAll<series_t>(prev, "E")));
			return nv;
		}
		case 'i': {
			auto nv = generatePossibilities(s.substr(1), addToAll<series_t>(prev, "i"));
			nv = combine(nv, generatePossibilities(s.substr(1), addToAll<series_t>(prev, "I")));
			return nv;
		}
		case 'j': {
			auto nv = generatePossibilities(s.substr(1), addToAll<series_t>(prev, "i"));
			nv = combine(nv, generatePossibilities(s.substr(1), addToAll<series_t>(prev, "I")));
			return nv;
		}
		case 'o': {
			auto nv = generatePossibilities(s.substr(1), addToAll<series_t>(prev, "o"));
			nv = combine(nv, generatePossibilities(s.substr(1), addToAll<series_t>(prev, "O")));
			return nv;
		}
		case 'u': {
			auto nv = generatePossibilities(s.substr(1), addToAll<series_t>(prev, "u"));
			nv = combine(nv, generatePossibilities(s.substr(1), addToAll<series_t>(prev, "U")));
			nv = combine(nv, generatePossibilities(s.substr(1), addToAll<series_t>(prev, "v")));
			return nv;
		}
		case 'v': {
			auto nv = generatePossibilities(s.substr(1), addToAll<series_t>(prev, "u"));
			nv = combine(nv, generatePossibilities(s.substr(1), addToAll<series_t>(prev, "U")));
			nv = combine(nv, generatePossibilities(s.substr(1), addToAll<series_t>(prev, "v")));
			return nv;
		}
		case 'y': {
			auto nv = generatePossibilities(s.substr(1), addToAll<series_t>(prev, "y"));
			nv = combine(nv, generatePossibilities(s.substr(1), addToAll<series_t>(prev, "Y")));
			return nv;
		}
		default:
			return generatePossibilities(s.substr(1), addToAll<series_t>(prev, std::string(1, s[0])));
	}
}

void recursivePrint(const SearchMap &map, const int &i)
{
	for (auto &l : map.lemmas) {
		for (int j = 0; j < i; j++)
			std::cout << "\t";
		std::cout << "NODE: " << canonicalForm(l) << "\n";
	}
	/*for (auto &l : map.noun_lemmas) {
		for (int j = 0; j < i; j++)
			std::cout << "\t";
		std::cout << "NOUN: " << canonicalForm(l) << "\n";
	}
	for (auto &l : map.adj_lemmas) {
		for (int j = 0; j < i; j++)
			std::cout << "\t";
		std::cout << "ADJ: " << canonicalForm(l) << "\n";
	}
	for (auto &l : map.verb_lemmas) {
		for (int j = 0; j < i; j++)
			std::cout << "\t";
		std::cout << "VERB: " << canonicalForm(l) << "\n";
	}*/
	for (auto &e : map.next) {
		for (int j = 0; j < i; j++)
			std::cout << "\t";
		std::cout << "MAP: " << e.first << "\n";
		recursivePrint(e.second, i + 1);
	}
}

int main()
{
#ifdef _WIN32
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);
#endif
	SearchMap search_map;
	readNouns(&search_map);
	readAdjs(&search_map);
	readVerbs(&search_map);
	//recursivePrint(search_map, 0);

	while (true) {
		std::string line;
		std::cout << "LAT> ";
		std::getline(std::cin, line);
		auto ps = generatePossibilities(line, { "" });
		std::vector<Node> fl;
		for (auto &p : ps) {
			fl = combine(fl, findLemmaSequence(p, &search_map));
		}
		for (auto &l : fl) {
			switch (l.type) {
				case NOUN:
					std::cout << CTEXT(parseSeries(decline(l.nounLemma, l.nounQuery.i)), BRIGHT_CYAN_TEXT) << "\t" << CTEXT(declensionName(l.nounQuery.i), MAGENTA_TEXT) << " of " << CTEXT(canonicalForm(l), BRIGHT_BLACK_TEXT) << " [NOUN]\n";
					break;
				case ADJECTIVE:
					std::cout << CTEXT(parseSeries(decline(l.adjLemma, l.adjQuery.i, l.adjQuery.g)), BRIGHT_CYAN_TEXT) << "\t" << CTEXT(declensionName(l.adjQuery.i), MAGENTA_TEXT) << " " << CTEXT(genderName(l.adjQuery.g), YELLOW_TEXT) << " of " << CTEXT(canonicalForm(l), BRIGHT_BLACK_TEXT) << " [ADJ]\n";
					break;
				case VERB:
					std::cout << CTEXT(parseSeries(conjugate(l.verbLemma, l.verbQuery.c)), BRIGHT_CYAN_TEXT) << "\t" << CTEXT(l.verbQuery.c, MAGENTA_TEXT) << " of " << CTEXT(canonicalForm(l), BRIGHT_BLACK_TEXT) << " [VERB]\n";
					break;
			}
		}
	}
	return 0;
}