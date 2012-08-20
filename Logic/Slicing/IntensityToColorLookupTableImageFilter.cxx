#include "IntensityToColorLookupTableImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "IntensityCurveInterface.h"
#include "ColorMap.h"
#include "itkOrientedImage.h"

template<class TInputImage, class TOutputLUT>
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::IntensityToColorLookupTableImageFilter()
{
  // The intensity curve and the color map are treated as inputs
  this->SetNumberOfInputs(5);

  // By default not using a reference range
  m_UseReferenceRange = false;
}

template<class TInputImage, class TOutputLUT>
void
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::SetIntensityCurve(IntensityCurveInterface *curve)
{
  m_IntensityCurve = curve;
  this->SetNthInput(1, curve);
}

template<class TInputImage, class TOutputLUT>
void
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::SetColorMap(ColorMap *map)
{
  m_ColorMap = map;
  this->SetNthInput(2, map);
}

template<class TInputImage, class TOutputLUT>
void
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::SetImageMinInput(InputPixelObject *input)
{
  m_InputMin = input;
  this->SetNthInput(3, input);
}

template<class TInputImage, class TOutputLUT>
void
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::SetImageMaxInput(InputPixelObject *input)
{
  m_InputMax = input;
  this->SetNthInput(4, input);
}

template<class TInputImage, class TOutputLUT>
void
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::SetReferenceIntensityRange(double min, double max)
{
  m_UseReferenceRange = true;
  m_ReferenceMin = min;
  m_ReferenceMax = max;
  this->Modified();
}

template<class TInputImage, class TOutputLUT>
void
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::RemoveReferenceIntensityRange()
{
  m_UseReferenceRange = false;
  this->Modified();
}

template<class TInputImage, class TOutputLUT>
void
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::GenerateOutputInformation()
{
  LookupTableType *output = this->GetOutput();
  typename LookupTableType::IndexType idx = {{m_InputMin->Get()}};
  typename LookupTableType::SizeType sz = {{1 + m_InputMax->Get() - m_InputMin->Get()}};
  typename LookupTableType::RegionType region(idx, sz);
  output->SetLargestPossibleRegion(region);
}

template<class TInputImage, class TOutputLUT>
void
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::EnlargeOutputRequestedRegion(itk::DataObject *)
{
  LookupTableType *output = this->GetOutput();
  output->SetRequestedRegion(output->GetLargestPossibleRegion());
}

template<class TInputImage, class TOutputLUT>
void
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::GenerateInputRequestedRegion()
{
  // The input region is the whole image
  InputImageType *input = const_cast<InputImageType *>(this->GetInput());
  input->SetRequestedRegionToLargestPossibleRegion();
}

template<class TInputImage, class TOutputLUT>
void
IntensityToColorLookupTableImageFilter<TInputImage, TOutputLUT>
::ThreadedGenerateData(const OutputImageRegionType &region, int threadId)
{
  if(threadId == 0)
    std::cout << "Computing LUT " << region << std::endl;

  // Get the image max and min
  InputPixelType imin = m_InputMin->Get(), imax = m_InputMax->Get();

  // Set the intensity mapping for the functor
  float scale, shift;
  if(m_UseReferenceRange)
    {
    shift = m_ReferenceMin;
    scale = 1.0f / (m_ReferenceMax - m_ReferenceMin);
    }
  else
    {
    shift = imin;
    scale = 1.0f / (imax - imin);
    }

  // Do the actual computation of the cache
  LookupTableType *output = this->GetOutput();
  for(itk::ImageRegionIteratorWithIndex<LookupTableType> it(output, region);
      !it.IsAtEnd(); ++it)
    {
    // Get the lookup value we are seeking
    long pos = it.GetIndex()[0];

    // Map the input value to range of 0 to 1
    float inZeroOne = (pos - shift) * scale;

    // Compute the intensity mapping
    float outZeroOne = m_IntensityCurve->Evaluate(inZeroOne);

    // Map the output to a RGBA pixel
    it.Set(m_ColorMap->MapIndexToRGBA(outZeroOne));
    }
}

// Template instantiation
typedef itk::OrientedImage<short, 3> GreyImageType;
typedef itk::Image<itk::RGBAPixel<unsigned char>, 1> LUTType;
template class IntensityToColorLookupTableImageFilter<GreyImageType, LUTType>;