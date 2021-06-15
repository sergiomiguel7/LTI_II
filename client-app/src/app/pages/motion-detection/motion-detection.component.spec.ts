import { ComponentFixture, TestBed } from '@angular/core/testing';

import { MotionDetectionComponent } from './motion-detection.component';

describe('MotionDetectionComponent', () => {
  let component: MotionDetectionComponent;
  let fixture: ComponentFixture<MotionDetectionComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ MotionDetectionComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(MotionDetectionComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
