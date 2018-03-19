-----------------------------------------------------------------------------
-- |
-- Module      :  IL2212.ImplModel.ImageProcessing
-- Copyright   :  (c) <your name>, 2018
-- License     :  BSD-style (see the file LICENSE)
-- 
-- Maintainer  :  <email>@kth.se
-- Stability   :  stable
-- Portability :  portable
--
-- Contains the image processing functions as well as the DUT process
-- network instantiation.
-----------------------------------------------------------------------------
module IL2212.ImplModel.ImageProcessing where

import ForSyDe.Shallow
import IL2212.ImageProcessing -- import the functions original image processing application
import IL2212.Utilities       -- import the utility functions defined in the original model

      
-- your code

-- | Data type capturing two states, used for control signals.
data MyControl = Enable | Disable deriving (Show, Eq)


myImageProcessing :: Int         -- ^ dimension X of the input image
                  -> Int         -- ^ dimension Y of the input image
                  -> Signal Int  -- ^ Input stream of pixel values
                  -> Signal Char -- ^ Output stream of ASCII characters
myImageProcessing dimX dimY =  cpu3 . cpu2 . cpu1 . cpu0
 where
    cpu0 = actor11SDF (x0 * y1) (x1 * y1) (wrapImageF x0 y1 grayscale )
    cpu2 = actor11SDF (x2 * y2) (x3 * y3) (wrapImageF x2 y2 sobel     )
    cpu3 = actor11SDF (x3 * y3) (x3 * y3) (wrapImageF x3 y3 toAsciiArt)
    cpu1 sig = correctSDF controlSig brightnessSig resizedSig
      where
        correctSDF    = actor31SDF (1, 2, x2 * y2) (x2 * y2) correctFunc
        resizedSig    = actor11SDF (x1 * y1) (x2 * y2) (wrapImageF x1 y1 resize) sig
        brightnessSig = actor11SDF (x2 * y2) 2 (brightness . toImage x2 y2) resizedSig
        controlSig    = mooreSDF
                          ((3,2), 3, nextStateFunc)
                          ( 3   , 1, outDecodeFunc 3)
                          [255,255,255]
                          brightnessSig
    ------------------------------------------------------------
    -- Process functions
    ------------------------------------------------------------
    correctFunc :: [MyControl] -> [Double] -> [Double] -> [Double]
    correctFunc [Disable] _           = wrapImageF x2 y2 id
    correctFunc [Enable] [hmin,hmax] = wrapImageF x2 y2 (correction hmin hmax)
    correctFunc _         _           = error "correctFunc: invalid process function"
    ------------------------------------------------------------
    nextStateFunc :: [Double] -> [Double] -> [Double] -- revolving buffer with lists
    nextStateFunc state [hmin, hmax] = (hmax-hmin) : init state
    nextStateFunc _     _            = error "nextStateFunc: invalid process function"
    ------------------------------------------------------------
    outDecodeFunc :: Int -> [Double] -> [MyControl]
    outDecodeFunc n levels = if (sum levels / fromIntegral n) < 128
                             then [Enable]
                             else [Disable]
    ------------------------------------------------------------
    -- Production / consumption rates
    ------------------------------------------------------------
    x0 = dimX * 3
    x1 = dimX
    y1 = dimY
    x2 = dimX `div` 2
    y2 = dimY `div` 2
    x3 = (dimX `div` 2) - 2
    y3 = (dimY `div` 2) - 2