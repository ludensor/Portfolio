// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EQSNavAgentInterface.h"
#include "NoriProjectCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ANoriProjectCharacter : public ACharacter, public IEQSNavAgentInterface
{
	GENERATED_BODY()

public:
	ANoriProjectCharacter();

	virtual void GetNavAgentProperties(float& OutAgentRadius, float& OutAgentHalfHeight) const final;

private:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) final;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveUp(float Value);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
};

