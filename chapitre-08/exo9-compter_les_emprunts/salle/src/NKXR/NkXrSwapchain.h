//
// NkXrSwapchain.h
// =============================================================================
// Description :
//   La swapchain XR : la file d'images d'UN Å“il, avec la discipline
//   Acquire â†’ rendre â†’ Release â†’ soumettre en couche.
//
// CaractÃ©ristiques :
//   - Ã€ l'Ã©tage 0 (simulateur), les images GPU appartiennent Ã  l'application
//     (cibles offscreen NKRenderer) : la swapchain ne porte que des handles
//     OPAQUES (uint64) posÃ©s par l'app. Ce n'est pas un trou dans le design,
//     c'est le design : Ã  l'Ã©tage 2 c'est le runtime OpenXR qui FOURNIT les
//     images, et ce qui doit Ãªtre en place cÃ´tÃ© application d'ici lÃ , c'est
//     prÃ©cisÃ©ment la discipline d'acquisition â€” elle, est dÃ©jÃ  rÃ©elle ici
//     (double Acquire refusÃ©, Release exigÃ© avant soumission).
//   - ZÃ©ro allocation : tableau d'images Ã  taille fixe. Quatre suffisent, les
//     runtimes rÃ©els en donnent 2 ou 3.
//
// Auteur   : Rihen
// Copyright: (c) 2024-2026 Rihen. Tous droits rÃ©servÃ©s.
// =============================================================================

#pragma once

#ifndef __NKENTSEU_XR_NKXRSWAPCHAIN_H__
#define __NKENTSEU_XR_NKXRSWAPCHAIN_H__

#include "NkXrTypes.h"

namespace nkentseu {
	namespace xr {

		struct NkXrSwapchainDesc {
			uint32 width = 0;
			uint32 height = 0;
			// 1 image = rendu direct dans une cible unique (le cas simulateur :
			// le compositeur lit la cible aprÃ¨s le rendu, pas en parallÃ¨le).
			uint32 imageCount = 1;
			NkXrEye eye = NkXrEye::NK_XR_EYE_LEFT;
			const char *name = "";
		};

		class NkXrSwapchain {
			public:
				static constexpr uint32 kMaxImages = 4u;

				bool Init(const NkXrSwapchainDesc &desc) noexcept {
					if (desc.width == 0u || desc.height == 0u) {
						return false;
					}
					if (desc.imageCount == 0u || desc.imageCount > kMaxImages) {
						return false;
					}
					mDesc = desc;
					mNextImage = 0u;
					mAcquired = false;
					for (uint32 i = 0u; i < kMaxImages; ++i) {
						mNativeImages[i] = 0u;
					}
					return true;
				}

				uint32 GetWidth() const noexcept { return mDesc.width; }
				uint32 GetHeight() const noexcept { return mDesc.height; }
				uint32 GetImageCount() const noexcept { return mDesc.imageCount; }
				NkXrEye GetEye() const noexcept { return mDesc.eye; }

				// L'application enregistre SES images GPU (handle opaque â€”
				// NkTexHandle, id RHI, peu importe : la swapchain ne les
				// interprÃ¨te pas, elle les restitue Ã  la composition).
				void SetImageNative(uint32 index, uint64 native) noexcept {
					if (index < mDesc.imageCount) {
						mNativeImages[index] = native;
					}
				}

				uint64 GetImageNative(uint32 index) const noexcept {
					return (index < mDesc.imageCount) ? mNativeImages[index] : 0u;
				}

				// Refuse le double-Acquire : c'est l'erreur que les runtimes
				// rÃ©els sanctionnent, autant qu'elle soit impossible dÃ¨s ici.
				bool AcquireImage(uint32 &outIndex) noexcept {
					if (mAcquired) {
						return false;
					}
					outIndex = mNextImage;
					mAcquired = true;
					return true;
				}

				bool ReleaseImage() noexcept {
					if (!mAcquired) {
						return false;
					}
					mNextImage = (mNextImage + 1u) % mDesc.imageCount;
					mAcquired = false;
					return true;
				}

				bool IsAcquired() const noexcept { return mAcquired; }

				// Index de la DERNIÃˆRE image relÃ¢chÃ©e : celle qu'une couche
				// peut lÃ©gitimement rÃ©fÃ©rencer Ã  EndFrame.
				uint32 GetLastReleasedIndex() const noexcept {
					return (mNextImage + mDesc.imageCount - 1u) % mDesc.imageCount;
				}

			private:
				NkXrSwapchainDesc mDesc{};
				uint64 mNativeImages[kMaxImages]{};
				uint32 mNextImage = 0u;
				bool mAcquired = false;
		};

	} // namespace xr
} // namespace nkentseu

#endif // __NKENTSEU_XR_NKXRSWAPCHAIN_H__

