#pragma once

#include <string>
#include <map>
#include <vector>

typedef std::string series_t;

enum Gender
{
	G_MAS,
	G_NEU,
	G_FEM
};

enum AType
{
	A_POS,
	A_COMP,
	A_SUPR
};

enum Inflection
{
	NOM_SG,
	NOM_PL,
	GEN_SG,
	GEN_PL,
	DAT_SG,
	DAT_PL,
	ACC_SG,
	ACC_PL,
	ABL_SG,
	ABL_PL,
	VOC_SG,
	VOC_PL,
	LOC_SG,
	LOC_PL
};

enum ConjugationSchema
{
	INF_ACT_PRE,
	INF_ACT_PRF,
	INF_PAS_PRE,

	// ----------------------------------------------

	IMP_ACT_PRE_2SG,
	IMP_ACT_PRE_2PL,
	IMP_ACT_FUT_2SG,
	IMP_ACT_FUT_3SG,
	IMP_ACT_FUT_2PL,
	IMP_ACT_FUT_3PL,

	IMP_PAS_PRE_2SG,
	IMP_PAS_PRE_2PL,
	IMP_PAS_FUT_2SG,
	IMP_PAS_FUT_3SG,
	IMP_PAS_FUT_3PL,

	// ----------------------------------------------

	IND_ACT_SIM_PRE_1SG,
	IND_ACT_SIM_PRE_2SG,
	IND_ACT_SIM_PRE_3SG,
	IND_ACT_SIM_PRE_1PL,
	IND_ACT_SIM_PRE_2PL,
	IND_ACT_SIM_PRE_3PL,

	IND_ACT_SIM_IMP_1SG,
	IND_ACT_SIM_IMP_2SG,
	IND_ACT_SIM_IMP_3SG,
	IND_ACT_SIM_IMP_1PL,
	IND_ACT_SIM_IMP_2PL,
	IND_ACT_SIM_IMP_3PL,

	IND_ACT_SIM_FUT_1SG,
	IND_ACT_SIM_FUT_2SG,
	IND_ACT_SIM_FUT_3SG,
	IND_ACT_SIM_FUT_1PL,
	IND_ACT_SIM_FUT_2PL,
	IND_ACT_SIM_FUT_3PL,

	// ----------------------------------------------

	IND_ACT_PRF_PRE_1SG,
	IND_ACT_PRF_PRE_2SG,
	IND_ACT_PRF_PRE_3SG,
	IND_ACT_PRF_PRE_1PL,
	IND_ACT_PRF_PRE_2PL,
	IND_ACT_PRF_PRE_3PL,

	IND_ACT_PRF_IMP_1SG,
	IND_ACT_PRF_IMP_2SG,
	IND_ACT_PRF_IMP_3SG,
	IND_ACT_PRF_IMP_1PL,
	IND_ACT_PRF_IMP_2PL,
	IND_ACT_PRF_IMP_3PL,

	IND_ACT_PRF_FUT_1SG,
	IND_ACT_PRF_FUT_2SG,
	IND_ACT_PRF_FUT_3SG,
	IND_ACT_PRF_FUT_1PL,
	IND_ACT_PRF_FUT_2PL,
	IND_ACT_PRF_FUT_3PL,

	// ----------------------------------------------

	IND_PAS_SIM_PRE_1SG,
	IND_PAS_SIM_PRE_2SG,
	IND_PAS_SIM_PRE_3SG,
	IND_PAS_SIM_PRE_1PL,
	IND_PAS_SIM_PRE_2PL,
	IND_PAS_SIM_PRE_3PL,

	IND_PAS_SIM_IMP_1SG,
	IND_PAS_SIM_IMP_2SG,
	IND_PAS_SIM_IMP_3SG,
	IND_PAS_SIM_IMP_1PL,
	IND_PAS_SIM_IMP_2PL,
	IND_PAS_SIM_IMP_3PL,

	IND_PAS_SIM_FUT_1SG,
	IND_PAS_SIM_FUT_2SG,
	IND_PAS_SIM_FUT_3SG,
	IND_PAS_SIM_FUT_1PL,
	IND_PAS_SIM_FUT_2PL,
	IND_PAS_SIM_FUT_3PL,

	// ----------------------------------------------

	SUB_ACT_SIM_PRE_1SG,
	SUB_ACT_SIM_PRE_2SG,
	SUB_ACT_SIM_PRE_3SG,
	SUB_ACT_SIM_PRE_1PL,
	SUB_ACT_SIM_PRE_2PL,
	SUB_ACT_SIM_PRE_3PL,

	SUB_ACT_SIM_IMP_1SG,
	SUB_ACT_SIM_IMP_2SG,
	SUB_ACT_SIM_IMP_3SG,
	SUB_ACT_SIM_IMP_1PL,
	SUB_ACT_SIM_IMP_2PL,
	SUB_ACT_SIM_IMP_3PL,

	// ----------------------------------------------

	SUB_ACT_PRF_PRE_1SG,
	SUB_ACT_PRF_PRE_2SG,
	SUB_ACT_PRF_PRE_3SG,
	SUB_ACT_PRF_PRE_1PL,
	SUB_ACT_PRF_PRE_2PL,
	SUB_ACT_PRF_PRE_3PL,

	SUB_ACT_PRF_IMP_1SG,
	SUB_ACT_PRF_IMP_2SG,
	SUB_ACT_PRF_IMP_3SG,
	SUB_ACT_PRF_IMP_1PL,
	SUB_ACT_PRF_IMP_2PL,
	SUB_ACT_PRF_IMP_3PL,

	// ----------------------------------------------

	SUB_PAS_SIM_PRE_1SG,
	SUB_PAS_SIM_PRE_2SG,
	SUB_PAS_SIM_PRE_3SG,
	SUB_PAS_SIM_PRE_1PL,
	SUB_PAS_SIM_PRE_2PL,
	SUB_PAS_SIM_PRE_3PL,

	SUB_PAS_SIM_IMP_1SG,
	SUB_PAS_SIM_IMP_2SG,
	SUB_PAS_SIM_IMP_3SG,
	SUB_PAS_SIM_IMP_1PL,
	SUB_PAS_SIM_IMP_2PL,
	SUB_PAS_SIM_IMP_3PL
};

enum NodeType
{
	NOUN,
	ADJECTIVE,
	VERB
};

struct DPair
{
	series_t sg = "*";
	series_t pl = "*";
};

struct Declension
{
	std::string name;
	DPair nom;
	DPair gen;
	DPair dat;
	DPair acc;
	DPair abl;
	DPair voc;
	DPair loc;
};

struct CTuple
{
	series_t _1sg = "*";
	series_t _2sg = "*";
	series_t _3sg = "*";
	series_t _1pl = "*";
	series_t _2pl = "*";
	series_t _3pl = "*";
};

struct Conjugation
{
	std::string name;
	series_t inf = "*";
	series_t imp1 = "*";
	series_t imp2 = "*";
	series_t imp3 = "*";
	CTuple ind_pres;
	CTuple ind_impf;
	CTuple ind_fut;
	CTuple sub_pres;
	CTuple sub_impf;
};

struct NounLemma
{
	series_t lemma = "*";
	series_t genov = "*";
	series_t stem = "*";
	Gender gender;
	Declension decl;
	std::string meaning;
};

struct AdjLemma
{
	AType type;
	series_t mlemma = "*";
	series_t flemma = "*";
	series_t nlemma = "*";
	series_t stem = "*";
	series_t suffix = "*";
	Declension mas;
	Declension fem;
	Declension neu;
	std::string meaning;
};

struct VerbLemma
{
	series_t sim_stem = "*";
	series_t prf_stem = "*";
	series_t extra_stem = "*";
	series_t sup_stem = "*";
	series_t ger_stem = "*";
	series_t prs_act_part_stem = "*";
	series_t prs_fut_part_stem = "*";
	Conjugation active_simple;
	Conjugation active_perfect;
	Conjugation passive_simple;
	std::string meaning;
};

struct NounQuery
{
	Inflection i;
};

struct AdjQuery
{
	Inflection i;
	Gender g;
};

struct VerbQuery
{
	ConjugationSchema c;
};

struct Node
{
	NodeType type;
	union
	{
		NounQuery nounQuery;
		AdjQuery adjQuery;
		VerbQuery verbQuery;
	};
	NounLemma nounLemma;
	AdjLemma adjLemma;
	VerbLemma verbLemma;

	Node(const NounLemma &, const NounQuery &);
	Node(const AdjLemma &, const AdjQuery &);
	Node(const VerbLemma &, const VerbQuery &);
};

struct SearchMap
{
	std::map<char, SearchMap> next;
	std::vector<Node> lemmas;
};

inline const bool operator==(const DPair &a, const DPair &b)
{
	if (a.sg != b.sg)
		return false;
	if (a.pl != b.pl)
		return false;
	return true;
}

inline const bool operator!=(const DPair &a, const DPair &b)
{
	return !(a == b);
}

inline const bool operator==(const CTuple &a, const CTuple &b)
{
	if (a._1sg != b._1sg)
		return false;
	if (a._2sg != b._2sg)
		return false;
	if (a._3sg != b._3sg)
		return false;
	if (a._1pl != b._1pl)
		return false;
	if (a._2pl != b._2pl)
		return false;
	if (a._3pl != b._3pl)
		return false;
	return true;
}

inline const bool operator!=(const CTuple &a, const CTuple &b)
{
	return !(a == b);
}

inline const bool operator==(const Declension &a, const Declension &b)
{
	if (a.name != b.name)
		return false;
	if (a.nom != b.nom)
		return false;
	if (a.gen != b.gen)
		return false;
	if (a.dat != b.dat)
		return false;
	if (a.acc != b.acc)
		return false;
	if (a.abl != b.abl)
		return false;
	if (a.voc != b.voc)
		return false;
	if (a.loc != b.loc)
		return false;
	return true;
}

inline const bool operator!=(const Declension &a, const Declension &b)
{
	return !(a == b);
}

inline const bool operator==(const Conjugation &a, const Conjugation &b)
{
	if (a.name != b.name)
		return false;
	if (a.inf != b.inf)
		return false;
	if (a.imp1 != b.imp1)
		return false;
	if (a.imp2 != b.imp2)
		return false;
	if (a.imp3 != b.imp3)
		return false;
	if (a.ind_pres != b.ind_pres)
		return false;
	if (a.ind_impf != b.ind_impf)
		return false;
	if (a.ind_fut != b.ind_fut)
		return false;
	if (a.sub_pres != b.sub_pres)
		return false;
	if (a.sub_impf != b.sub_impf)
		return false;
	return true;
}

inline const bool operator!=(const Conjugation &a, const Conjugation &b)
{
	return !(a == b);
}

inline const bool operator==(const NounLemma &a, const NounLemma &b)
{
	if (a.lemma != b.lemma)
		return false;
	if (a.stem != b.stem)
		return false;
	if (a.gender != b.gender)
		return false;
	if (a.decl != b.decl)
		return false;
	if (a.meaning != b.meaning)
		return false;
	return true;
}

inline const bool operator!=(const NounLemma &a, const NounLemma &b)
{
	return !(a == b);
}

inline const bool operator==(const AdjLemma &a, const AdjLemma &b)
{
	if (a.mlemma != b.mlemma)
		return false;
	if (a.flemma != b.flemma)
		return false;
	if (a.nlemma != b.nlemma)
		return false;
	if (a.stem != b.stem)
		return false;
	if (a.suffix != b.suffix)
		return false;
	if (a.mas != b.mas)
		return false;
	if (a.fem != b.fem)
		return false;
	if (a.neu != b.neu)
		return false;
	if (a.meaning != b.meaning)
		return false;
	return true;
}

inline const bool operator!=(const AdjLemma &a, const AdjLemma &b)
{
	return !(a == b);
}

inline const bool operator==(const VerbLemma &a, const VerbLemma &b)
{
	if (a.sim_stem != b.sim_stem)
		return false;
	if (a.prf_stem != b.prf_stem)
		return false;
	if (a.extra_stem != b.extra_stem)
		return false;
	if (a.sup_stem != b.sup_stem)
		return false;
	if (a.ger_stem != b.ger_stem)
		return false;
	if (a.prs_act_part_stem != b.prs_act_part_stem)
		return false;
	if (a.prs_fut_part_stem != b.prs_fut_part_stem)
		return false;
	if (a.active_simple != b.active_simple)
		return false;
	if (a.active_perfect != b.active_perfect)
		return false;
	if (a.passive_simple != b.passive_simple)
		return false;
	if (a.meaning != b.meaning)
		return false;
	return true;
}

inline const bool operator!=(const VerbLemma &a, const VerbLemma &b)
{
	return !(a == b);
}

inline const bool operator==(const NounQuery &a, const NounQuery &b)
{
	if (a.i != b.i)
		return false;
	return true;
}

inline const bool operator!=(const NounQuery &a, const NounQuery &b)
{
	return !(a == b);
}

inline const bool operator==(const AdjQuery &a, const AdjQuery &b)
{
	if (a.i != b.i)
		return false;
	if (a.g != b.g)
		return false;
	return true;
}

inline const bool operator!=(const AdjQuery &a, const AdjQuery &b)
{
	return !(a == b);
}

inline const bool operator==(const VerbQuery &a, const VerbQuery &b)
{
	if (a.c != b.c)
		return false;
	return true;
}

inline const bool operator!=(const VerbQuery &a, const VerbQuery &b)
{
	return !(a == b);
}

inline const bool operator==(const Node &a, const Node &b)
{
	if (a.type != b.type)
		return false;
	switch (a.type) {
		case NOUN:
			if (a.nounQuery != b.nounQuery)
				return false;
			if (a.nounLemma != b.nounLemma)
				return false;
			break;
		case ADJECTIVE:
			if (a.adjQuery != b.adjQuery)
				return false;
			if (a.adjLemma != b.adjLemma)
				return false;
			break;
		case VERB:
			if (a.verbQuery != b.verbQuery)
				return false;
			if (a.verbLemma != b.verbLemma)
				return false;
			break;
	}
	return true;
}

inline const bool operator!=(const Node &a, const Node &b)
{
	return !(a == b);
}