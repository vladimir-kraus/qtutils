//
// Copyright (c) Vladimir Kraus. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
// https://github.com/vladimir-kraus/qtutils
//

/*
 * SafePointer is a strong pointer specialized for QObject-based classes.
 * SafePointer owns the object in the sense that when the pointer gets destroyed,
 * it deletes the owned object.
 *
 * On ther other hand it is safe like a weak pointer. When someone else
 * deletes the owned object, the pointer ownership is nullified.
 * This means there is never a double-delete situation. Therefore it
 * complements very well with parent-child ownership or
 * with any other kind of ownership.
 *
 * This means that the object can have multiple owners but the object
 * gets deleted when the first owner is destroyed. This is in contrast to
 * shared_ptr where the owned object gets deleted when the last owned
 * is destroyed.
 */
template <typename T>
class SafePointer : public QPointer<T>
{
public:
    /*
     * Becomes the owner of the object.
     */
    SafePointer(T *obj = nullptr) : QPointer<T>(obj)
    { }

    /*
     * Disallowing copy constructor.
     *
     * It probably does not make sense to have copy
     * constructor. It would lead to incorrect usage.
     */
    SafePointer(const SafePointer &other) = delete;

    /*
     * Disallowing copy assignment.
     *
     * It probably does not make sense to have copy
     * constructor. It would lead to incorrect usage.
     */
    SafePointer<T> &operator=(SafePointer &other) = delete;

    /*
     * Conversion assignment from raw poiner.
     * Deletes the currently owned object (if there is any)
     * and becomes the owner of the new object.
     */
    SafePointer<T> &operator=(T *obj)
    {
        delete *this;
        QPointer<T>::operator=(obj);
        return *this;
    }

    /*
     * Return the raw pointer of the owned object and clear
     * the SafePointer without deleting the owned object.
     */
    T *release()
    {
        T *p = data();
        clear();
        return p;
    }

    /*
     * Deletes the owned object (if there is any).
     */
    void reset()
    {
        delete *this;
    }

    /*
     * Calls deleteLater() on owned object (if there is any).
     */
    void resetLater()
    {
        if (data() != nullptr)
        {
            data()->deleteLater();
        }
    }

    /*
     * Deletes the owned object (if there is any).
     */
    ~SafePointer()
    {
        delete *this;
    }
};
