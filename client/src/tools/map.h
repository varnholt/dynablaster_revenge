// template map container using red/black tree
// maps key -> value

#pragma once

#include "array.h"

template<class KeyType, class ValueType>
class Map
{
public:
   class Node
   {
   public:
      // constructor
      Node(const KeyType& key, const ValueType& value)
      : mLeft(0)
      , mRight(0)
      , mParent(0)
      , mColor(0)
      , mKey(key)
      , mValue(value)
      {
      }

      bool isBlack() const
      {
         return mColor == 'B';
      }

      bool isRed() const
      {
         return mColor == 'R';
      }

      void setRed()
      {
         mColor = 'R';
      }

      void setBlack()
      {
         mColor = 'B';
      }

      void setColor(char color)
      {
         mColor= color;
      }

      char color() const
      {
         return mColor;
      }

      Node* next()
      {
         Node* x= (Node*)this;
         if ( x->mRight && x->mRight->mParent != 0 )
         {
            x = x->mRight;

            while ( x->mLeft && x->mLeft->mParent != 0 )
            {
               x = x->mLeft;
            }

            return x;
         }

         Node* y = x->mParent;

         while ( y->mParent != 0 && x == y->mRight )
         {
            x = y;
            y = y->mParent;
         }

         if (y->mKey > mKey)
            return y;
         else
            return 0;
      }

      Node*     mLeft;
      Node*     mRight;
      Node*     mParent;
      char      mColor;
      KeyType   mKey;
      ValueType mValue;
   };

   class Iterator
   {
   public:
      Iterator()
      : mNode( 0 )
      {
      }

      Iterator(Node* node)
      : mNode( node )
      {
      }

      const KeyType& key() const
      {
         return mNode->mKey;
      }

      const ValueType& value() const
      {
         return mNode->mValue;
      }

      Node* node() const
      {
         return mNode;
      }

      Iterator& operator ++ (int)
      {
         mNode= mNode->next();
         return *this;
      }

      bool operator != (const Iterator& other)
      {
         return (mNode != other.node());
      }

   private:
      Node* mNode;
   };

   // constructor
   Map()
   : mRoot(0)
   {
   }

   // destructor
   ~Map()
   {
      postOrderDelete( mRoot );
   }

   Iterator begin() const
   {
      Node *node= mRoot;
      while (node->mLeft) node= node->mLeft;
      return Iterator(node);
   }

   Iterator find( const KeyType& key ) const
   {
      Node *node = findNode( key );
      return Iterator(node);
   }

   Iterator end() const
   {
//      Node *node= mRoot;
//      while (node->mRight) node= node->mRight;
      return Iterator(0);
   }

   // get list of values
   Array<ValueType> valueList() const
   {
      Array<ValueType> list;
      traverseValues( mRoot, list );
      return list;
   }

   // get list of keys
   Array<KeyType> keyList() const
   {
      Array<KeyType> list;
      traverseKeys( mRoot, list );
      return list;
   }

   // map contains key?
   bool contains( const KeyType& key )
   {
      Node* x = findNode( key );
      return (x != 0);
   }

   // insert key
   bool insert( const KeyType& key, const ValueType& value )
   {
      Node *x, *y, *z;

      y = 0;
      x = mRoot;

      while ( x != 0 )
      {
         y = x;       
         if ( key < x->mKey )
            x = x->mLeft;       
         else if ( key > x->mKey )
            x = x->mRight;
         else
         {
            x->mValue= value;
            return false;
         }
      }

      z = new Node( key, value );
      z->mParent = y;
      z->setRed();

      if ( y == 0 )
         mRoot = z;
      else
      {       
         if ( key < y->mKey )
            y->mLeft = z;
         else
            y->mRight = z;
      }

      fixup( z );

      return true;
   }

   // remove key
   bool remove( const KeyType& key )
   {
      Node *x= findNode( key );
      if (x == 0)
         return false;
      remove( x );
      delete x;
   }


private:

   Node* findNode( const KeyType& key ) const
   {
      Node *x = mRoot;

      while ( x != 0 )
      {
         if ( key < x->mKey )
            x = x->mLeft;       
         else if ( key > x->mKey )
            x = x->mRight;
         else
            return x;
      }

      return 0;
   }


   // delete key
   bool remove( Node *z )
   {
      Node *x, *y;

      if ( z->mLeft == 0 || z->mRight == 0 )
         y = z;
      else
         y = z->next();

      if ( y->mLeft != 0 )
         x = y->mLeft;
      else
         x = y->mRight;

      x->mParent = y->mParent;
      if ( y->mParent == 0 )
         mRoot = x;
      else
      {
         if ( y == y->mParent->mLeft )
            y->mParent->mLeft = x;
         else
            y->mParent->mRight = x;
      }

      if ( y != z )
         y->set( z );

      if ( y->isBlack() )
         delete_fixup( x );

      return true;
   }


   // left rotate
   int leftRotate( Node *x )
   {
      Node *y;
      y = x->mRight;
      x->mRight = y->mLeft;
      if (y->mLeft)
         y->mLeft->mParent = x;
      y->mParent = x->mParent;

      if ( x->mParent == 0 )
         mRoot = y;
      else if ( x == x->mParent->mLeft )
         x->mParent->mLeft = y;
      else
         x->mParent->mRight = y;
      y->mLeft = x;
      x->mParent = y;
      return 0;
   }


   // right rotate
   int rightRotate( Node *x )
   {
      Node *y;

      y = x->mLeft;
      x->mLeft = y->mRight;
      if (y->mRight)
         y->mRight->mParent = x;
      y->mParent = x->mParent;

      if ( x->mParent == 0 )
         mRoot = y;
      else if ( x == x->mParent->mRight )
         x->mParent->mRight = y;
      else
         x->mParent->mLeft = y;
      y->mRight = x;
      x->mParent = y;
      return 0;
   }


   // fix up
   int fixup( Node *z )
   {
      Node *y;

      while ( z->mParent && z->mParent->isRed() )
      {
         if ( z->mParent == z->mParent->mParent->mLeft )
         {
            y = z->mParent->mParent->mRight;

            if ( y && y->isRed() )
            {
               z->mParent->setBlack();
               y->setBlack();
               z->mParent->mParent->setRed();
               z = z->mParent->mParent;
            }
            else
            {
               if ( z == z->mParent->mRight )
               {
                  z = z->mParent;
                  leftRotate( z );
               }

               z->mParent->setBlack();
               z->mParent->mParent->setRed();;
               rightRotate( z->mParent->mParent );
            }
         }
         else
         {
            y = z->mParent->mParent->mLeft;

            if ( y && y->isRed() )
            {
               z->mParent->setBlack();
               y->setBlack();
               z->mParent->mParent->setRed();
               z = z->mParent->mParent;
            }
            else
            {
               if ( z == z->mParent->mLeft )
               {
                  z = z->mParent;
                  rightRotate( z );
               }
               z->mParent->setBlack();
               z->mParent->mParent->setRed();
               leftRotate( z->mParent->mParent );
            }
         }
      }

      mRoot->setBlack();

      y = 0;
      return 0;
   }


   // delete fixup
   int delete_fixup( Node *x )
   {
      Node *w;

      while ( x != mRoot && x->isBlack() )
      {
         if ( x == x->mParent->mLeft )
         {
            w = x->mParent->mRight;

            if ( w->isRed() )
            {
               w->setBlack();
               x->mParent->setRed();
               leftRotate( x->mParent );
               w = x->mParent->mRight;
            }

            if ( w->mLeft->isBlack() && w->mRight->isBlack() )
            {
               w->setRed();
               x = x->mParent;
            }
            else if ( w->mRight->isBlack() )
            {
               w->mLeft->setBlack();
               w->setRed();
               rightRotate( w );
               w = x->mParent->mRight;
            }

            w->setColor( x->mParent->color() );
            x->mParent->setBlack();
            w->mRight->setBlack();
            leftRotate( x->mParent );
            x = mRoot;
         }
         else
         {
            w = x->mParent->mLeft;

            if ( w->isRed() )
            {
               w->setBlack();
               x->mParent->setRed();
               rightRotate( x->mParent );
               w = x->mParent->mLeft;
            }

            if ( w->mRight->isBlack() && w->mLeft->isBlack() )
            {
               w->setRed();
               x = x->mParent;
            }
            else if ( w->mLeft->isBlack() )
            {
               w->mRight->setBlack();
               w->setRed();
               leftRotate( w );
               w = x->mParent->mLeft;
            }

            w->setColor( x->mParent->color() );
            x->mParent->setBlack();
            w->mLeft->setBlack();
            rightRotate( x->mParent );
            x = mRoot;
         }
      }

      x->setBlack();

      return 0;
   }

   // in-order traverse
   void traverseValues( Node *n, Array<ValueType>& list ) const
   {
      if ( n != 0 )
      {   
         traverseValues( n->mLeft, list );
         list.add( n->mValue );
         traverseValues( n->mRight, list );
      }   
   }

   // in-order traverse
   void traverseKeys( Node *n, Array<KeyType>& list ) const
   {
      if ( n != 0 )
      {   
         traverseKeys( n->mLeft, list );
         list.add( n->mKey );
         traverseKeys( n->mRight, list );
      }   
   }

   // recursive delete of all nodes
   void postOrderDelete( Node *n )
   {
      if ( n != 0 )   
      {   
         postOrderDelete( n->mLeft );       
         postOrderDelete( n->mRight );
         delete n;
      }
   }

private:
    Node* mRoot;    /*root*/
};

