/***************************************************************************************************
 **
 **  Copyright (C) 2013 Ingo Berg
 **
 **  Permission is hereby granted, free of charge, to any person obtaining a copy of this
 **  software and associated documentation files (the "Software"), to deal in the Software
 **  without restriction, including without limitation the rights to use, copy, modify,
 **  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 **  permit persons to whom the Software is furnished to do so, subject to the following conditions:
 **
 **  The above copyright notice and this permission notice shall be included in all copies or
 **  substantial portions of the Software.
 **
 **  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 **  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 **  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 **  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 **  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **
 ******************************************************************************************************/

#ifndef QMUQPARSERBASE_H
#define QMUQPARSERBASE_H

#include <limits.h>
#include <qcompilerdetection.h>
#include <QChar>
#include <QMap>
#include <QStack>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QtGlobal>
#include <memory>
#include <string>
#include <QLocale>

#include "qmuparserbytecode.h"
#include "qmuparsercallback.h"
#include "qmuparserdef.h"
#include "qmuparsererror.h"
#include "qmuparsertoken.h"
#include "qmuparsertokenreader.h"

template <class T> class QStack;

namespace qmu
{
/**
 * @file
 * @brief This file contains the class definition of the qmuparser engine.
 */

/**
 * @brief Mathematical expressions parser (base parser engine).
 * @author (C) 2013 Ingo Berg
 *
 * This is the implementation of a bytecode based mathematical expressions parser.
 * The formula will be parsed from string and converted into a bytecode.
 * Future calculations will be done with the bytecode instead the formula string
 * resulting in a significant performance increase.
 * Complementary to a set of internally implemented functions the parser is able to handle
 * user defined functions and variables.
 */
class QmuParserBase
{
    friend class QmuParserTokenReader;
public:
    QmuParserBase();
    explicit QmuParserBase(const QmuParserBase &a_Parser);
    QmuParserBase& operator=(const QmuParserBase &a_Parser);
    virtual ~QmuParserBase();

    static void        EnableDebugDump(bool bDumpCmd, bool bDumpStack);
    qreal              Eval() const;
    qreal*             Eval(int &nStackSize) const;
    void               Eval(qreal *results, int nBulkSize) const;
    int                GetNumResults() const;
    void               SetExpr(const QString &a_sExpr);
    void               SetVarFactory(facfun_type a_pFactory, void *pUserData = nullptr);
    void               ResetLocale();
    void               EnableOptimizer(bool a_bIsOn=true);
    void               EnableBuiltInOprt(bool a_bIsOn=true);
    bool               HasBuiltInOprt() const;
    void               AddValIdent(identfun_type a_pCallback);
    void               DefineOprt(const QString &a_sName, fun_type2 a_pFun, unsigned a_iPrec=0,
                                  EOprtAssociativity a_eAssociativity = oaLEFT, bool a_bAllowOpt = false);
    void               DefineConst(const QString &a_sName, qreal a_fVal);
    void               DefineStrConst(const QString &a_strName, const QString &a_strVal);
    void               DefineVar(const QString &a_sName, qreal *a_pVar);
    void               DefinePostfixOprt(const QString &a_sFun, fun_type1 a_pFun, bool a_bAllowOpt=true);
    void               DefineInfixOprt(const QString &a_sName, fun_type1 a_pFun, int a_iPrec=prINFIX,
                                       bool a_bAllowOpt=true);
    // Clear user defined variables, constants or functions
    void               ClearVar();
    void               ClearFun();
    void               ClearConst();
    void               ClearInfixOprt();
    void               ClearPostfixOprt();
    void               ClearOprt();
    void               RemoveVar(const QString &a_strVarName);
    const varmap_type& GetUsedVar() const;
    const varmap_type& GetVar() const;
    const valmap_type& GetConst() const;
    const QString&     GetExpr() const;
    const funmap_type& GetFunDef() const;
    static QString     GetVersion(EParserVersionInfo eInfo = pviFULL);
    static const QStringList& GetOprtDef();
    QMap<int, QString> GetTokens() const;
    QMap<int, QString> GetNumbers() const;
    void               DefineNameChars(const QString &a_szCharset);
    void               DefineOprtChars(const QString &a_szCharset);
    void               DefineInfixOprtChars(const QString &a_szCharset);
    const QString&     ValidNameChars() const;
    const QString&     ValidOprtChars() const;
    const QString&     ValidInfixOprtChars() const;
    void               SetArgSep(char_type cArgSep);
    QChar              GetArgSep() const;
    void Q_NORETURN    Error(EErrorCodes a_iErrc, int a_iPos = -1, const QString &a_sTok = QString() ) const;

    template<typename T>
    void DefineFun(const QString &a_strName, T a_pFun, bool a_bAllowOpt = true);

    void setAllowSubexpressions(bool value);

    QLocale getLocale() const;
    void    setLocale(const QLocale &value);

    QChar getDecimalPoint() const;
    void  setDecimalPoint(const QChar &c);

    QChar getThousandsSeparator() const;
    void  setThousandsSeparator(const QChar &c);

protected:
    static const QStringList c_DefaultOprt;
    QLocale m_locale;///< The locale used by the parser
    QChar m_decimalPoint;
    QChar m_thousandsSeparator;
    static bool g_DbgDumpCmdCode;
    static bool g_DbgDumpStack;
    void Init();
    virtual void InitCharSets() = 0;
    virtual void InitFun() = 0;
    virtual void InitConst() = 0;
    virtual void InitOprt() = 0;
    virtual void OnDetectVar(const QString &pExpr, int &nStart, int &nEnd);
    /**
     * @brief A facet class used to change decimal and thousands separator.
     */
    template<class TChar>
    class change_dec_sep : public std::numpunct<TChar>
    {
        public:
            explicit change_dec_sep(char_type cDecSep, char_type cThousandsSep = 0, int nGroup = 3)
            :std::numpunct<TChar>(), m_nGroup(nGroup), m_cDecPoint(cDecSep), m_cThousandsSep(cThousandsSep)
            {}
        protected:
            virtual char_type do_decimal_point() const Q_DECL_OVERRIDE
            {
                return m_cDecPoint;
            }

            virtual char_type do_thousands_sep() const Q_DECL_OVERRIDE
            {
                return m_cThousandsSep;
            }

            virtual std::string do_grouping() const Q_DECL_OVERRIDE
            {
                // fix for issue 4: https://code.google.com/p/muparser/issues/detail?id=4
                // courtesy of Jens Bartsch
                // original code:
                //        return std::string(1, (char)m_nGroup);
                // new code:
                return std::string(1, static_cast<char>(m_cThousandsSep > 0 ? m_nGroup : CHAR_MAX));
            }
        private:
            int m_nGroup;
            char_type m_cDecPoint;
            char_type m_cThousandsSep;
    };
private:
    /**
     * @brief Typedef for the parse functions.
     *
     * The parse function do the actual work. The parser exchanges
     * the function pointer to the parser function depending on
     * which state it is in. (i.e. bytecode parser vs. string parser)
     */
    typedef qreal (QmuParserBase::*ParseFunction)() const;

    /**
     * @brief Type used for storing an array of values.
     */
    typedef QVector<qreal> valbuf_type;

    /**
     * @brief Type for a vector of strings.
     */
    typedef QVector<QString> stringbuf_type;

    /**
     * @brief Typedef for the token reader.
     */
    typedef QmuParserTokenReader token_reader_type;

    /**
     * @brief Type used for parser tokens.
     */
    typedef QmuParserToken<qreal, QString> token_type;

    /**
     * @brief Maximum number of threads spawned by OpenMP when using the bulk mode.
     */
    static const int s_MaxNumOpenMPThreads = 4;

    /**
     * @brief Pointer to the parser function.
     *
     * Eval() calls the function whose address is stored there.
     */
    mutable ParseFunction     m_pParseFormula;
    mutable QmuParserByteCode m_vRPN;        ///< The Bytecode class.
    mutable stringbuf_type    m_vStringBuf; ///< String buffer, used for storing string function arguments
    stringbuf_type            m_vStringVarBuf;

    std::unique_ptr<token_reader_type> m_pTokenReader; ///< Managed pointer to the token reader object.

    funmap_type  m_FunDef;         ///< Map of function names and pointers.
    funmap_type  m_PostOprtDef;    ///< Postfix operator callbacks
    funmap_type  m_InfixOprtDef;   ///< unary infix operator.
    funmap_type  m_OprtDef;        ///< Binary operator callbacks
    valmap_type  m_ConstDef;       ///< user constants.
    strmap_type  m_StrVarDef;      ///< user defined string constants
    varmap_type  m_VarDef;         ///< user defind variables.

    bool m_bBuiltInOp;             ///< Flag that can be used for switching built in operators on and off

    QString m_sNameChars;      ///< Charset for names
    QString m_sOprtChars;      ///< Charset for postfix/ binary operator tokens
    QString m_sInfixOprtChars; ///< Charset for infix operator tokens

    mutable int m_nIfElseCounter;  ///< Internal counter for keeping track of nested if-then-else clauses

    // items merely used for caching state information
    mutable valbuf_type m_vStackBuffer; ///< This is merely a buffer used for the stack in the cmd parsing routine
    mutable int m_nFinalResultIdx;
    mutable QMap<int, QString> m_Tokens;///< Keep all tokens that we can translate
    mutable QMap<int, QString> m_Numbers;///< Keep all numbers what exist in formula

    bool allowSubexpressions;

    void               Assign(const QmuParserBase &a_Parser);
    void               InitTokenReader();
    void               ReInit() const;
    void               AddCallback(const QString &a_strName, const QmuParserCallback &a_Callback,
                                   funmap_type &a_Storage, const QString &a_szCharSet );
    void               ApplyRemainingOprt(QStack<token_type> &a_stOpt, QStack<token_type> &a_stVal) const;
    void               ApplyBinOprt(QStack<token_type> &a_stOpt, QStack<token_type> &a_stVal) const;
    void               ApplyIfElse(QStack<token_type> &a_stOpt, QStack<token_type> &a_stVal) const;
    void               ApplyFunc(QStack<token_type> &a_stOpt, QStack<token_type> &a_stVal, int iArgCount) const;
    token_type         ApplyStrFunc(const token_type &a_FunTok, const QVector<token_type> &a_vArg) const;
    int                GetOprtPrecedence(const token_type &a_Tok) const;
    EOprtAssociativity GetOprtAssociativity(const token_type &a_Tok) const;
    void               CreateRPN() const;
    qreal              ParseString() const;
    qreal              ParseCmdCode() const;
    qreal              ParseCmdCodeBulk(int nOffset, int nThreadID) const;
    // cppcheck-suppress functionStatic
    void               CheckName(const QString &a_sName, const QString &a_szCharSet) const;
    // cppcheck-suppress functionStatic
    void               CheckOprt(const QString &a_sName, const QmuParserCallback &a_Callback,
                                 const QString &a_szCharSet) const;
    void               StackDump(const QStack<token_type > &a_stVal, const QStack<token_type > &a_stOprt) const;
};

//---------------------------------------------------------------------------------------------------------------------
/**
 * @fn void qmu::QmuParserBase::DefineFun(const string_type &a_strName, fun_type0 a_pFun,
 * bool a_bAllowOpt = true)
 * @brief Define a parser function without arguments.
 * @param a_strName Name of the function
 * @param a_pFun Pointer to the callback function
 * @param a_bAllowOpt A flag indicating this function may be optimized
 */
template<typename T>
inline void QmuParserBase::DefineFun(const QString &a_strName, T a_pFun, bool a_bAllowOpt)
{
    AddCallback( a_strName, QmuParserCallback(a_pFun, a_bAllowOpt), m_FunDef, ValidNameChars() );
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Initialize the token reader.
 *
 * Create new token reader object and submit pointers to function, operator, constant and variable definitions.
 *
 * @post m_pTokenReader.get()!=0
 */
inline void QmuParserBase::InitTokenReader()
{
    m_pTokenReader.reset(new token_reader_type(this));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Add a value parsing function.
 *
 * When parsing an expression muParser tries to detect values in the expression string using different valident
 * callbacks. Thuis it's possible to parse for hex values, binary values and floating point values.
 */
inline void QmuParserBase::AddValIdent(identfun_type a_pCallback)
{
    m_pTokenReader->AddValIdent(a_pCallback);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Get the default symbols used for the built in operators.
 * @sa c_DefaultOprt
 */
inline const QStringList &QmuParserBase::GetOprtDef()
{
    return c_DefaultOprt;
}

//---------------------------------------------------------------------------------------------------------------------
inline QMap<int, QString> QmuParserBase::GetTokens() const
{
    return m_Tokens;
}

//---------------------------------------------------------------------------------------------------------------------
inline QMap<int, QString> QmuParserBase::GetNumbers() const
{
    return m_Numbers;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Define the set of valid characters to be used in names of functions, variables, constants.
 */
inline void QmuParserBase::DefineNameChars(const QString &a_szCharset)
{
    m_sNameChars = a_szCharset;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Define the set of valid characters to be used in names of binary operators and postfix operators.
 */
inline void QmuParserBase::DefineOprtChars(const QString &a_szCharset)
{
    m_sOprtChars = a_szCharset;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Define the set of valid characters to be used in names of infix operators.
 */
inline void QmuParserBase::DefineInfixOprtChars(const QString &a_szCharset)
{
    m_sInfixOprtChars = a_szCharset;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Return a map containing the used variables only.
 */
inline const varmap_type &QmuParserBase::GetVar() const
{
    return m_VarDef;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Return a map containing all parser constants.
 */
inline const valmap_type &QmuParserBase::GetConst() const
{
    return m_ConstDef;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Return prototypes of all parser functions.
 * @return #m_FunDef
 * @sa FunProt
 * @throw nothrow
 *
 * The return type is a map of the public type #funmap_type containing the prototype definitions for all numerical
 * parser functions. String functions are not part of this map. The Prototype definition is encapsulated in objects
 * of the class FunProt one per parser function each associated with function names via a map construct.
 */
inline const funmap_type &QmuParserBase::GetFunDef() const
{
    return m_FunDef;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve the formula.
 */
inline const QString& QmuParserBase::GetExpr() const
{
    return m_pTokenReader->GetExpr();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Query status of built in variables.
 * @return #m_bBuiltInOp; true if built in operators are enabled.
 * @throw nothrow
 */
inline bool QmuParserBase::HasBuiltInOprt() const
{
    return m_bBuiltInOp;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Return the number of results on the calculation stack.
 *
 * If the expression contains comma seperated subexpressions (i.e. "sin(y), x+y"). There mey be more than one return
 * value. This function returns the number of available results.
 */
// cppcheck-suppress unusedFunction
inline int QmuParserBase::GetNumResults() const
{
    return m_nFinalResultIdx;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Calculate the result.
 *
 * A note on const correctness:
 * I consider it important that Calc is a const function.
 * Due to caching operations Calc changes only the state of internal variables with one exception
 * m_UsedVar this is reset during string parsing and accessible from the outside. Instead of making
 * Calc non const GetUsedVar is non const because it explicitely calls Eval() forcing this update.
 *
 * @pre A formula must be set.
 * @pre Variables must have been set (if needed)
 *
 * @sa #m_pParseFormula
 * @return The evaluation result
 * @throw ParseException if no Formula is set or in case of any other error related to the formula.
 */
inline qreal QmuParserBase::Eval() const
{
    return (this->*m_pParseFormula)();
}

} // namespace qmu

#endif
