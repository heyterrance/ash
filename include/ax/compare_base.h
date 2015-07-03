// Comparison base classes
//

namespace ax {

template<class Base>
struct compareable
{
    template<class R>
    friend constexpr
    bool operator!=(const Base& lhs, const R& rhs)
    {
        return not (lhs == rhs);
    }

    template<class R>
    friend constexpr
    bool operator>=(const Base& lhs, const R& rhs)
    {
        return not (lhs < rhs);
    }

    template<class R>
    friend constexpr
    bool operator>(const Base& lhs, const R& rhs)
    {
        return not (lhs <= rhs);
    }

    template<class R>
    friend constexpr
    bool operator<=(const Base& lhs, const R& rhs)
    {
        return (lhs < rhs) or (lhs == rhs);
    }
};

} // namespace ax
